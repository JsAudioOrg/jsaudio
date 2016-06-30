'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

/* CALLBACK SINE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__sine_8c_source.html
*/

// Setup
const numSeconds = process.argv[2] || 5
const sampleRate = 48000
const channels = 2
const framesPerBuffer = 8192
const streamFlags = 0
const tableSize = 200
const formats = {
  paFloat32: 1,
  paInt32: 2,
  paInt24: 4,
  paInt16: 8,
  paInt8: 16,
  paUInt8: 32,
  paCustomFormat: 65536,
  paNonInterleaved: 2147483648
}


  
function callback (input, output, frameCount, data) {
  var outputBufferView = new Float32Array(output)

  for(var i=0; i<frameCount*2; i+=2 )
  {
    outputBufferView[i] = data.sine[data.left_phase]  // left 
    outputBufferView[i+1] = data.sine[data.right_phase]  // right 
    data.left_phase += 1
    if( data.left_phase >= data.tableSize ) data.left_phase -= data.tableSize
    data.right_phase += 3 // higher pitch so we can distinguish left and right.
    if( data.right_phase >= data.tableSize ) data.right_phase -= data.tableSize
  }
  return 0
}

//Main function
function callbackSine () {
  // initialize stream instance
  let stream = new JsPaStream()
  // initialize data that is sent to callback
  let data = {
    left_phase: 0,
    right_phase: 0,
    tableSize: tableSize,
    sine: []
  }
  console.log(
    'PortAudio Test: output sine wave\n',
    `SR = ${sampleRate}, BufSize = ${framesPerBuffer}\n`
  )
  
  // initialise sinusoidal wavetable
  for(var i=0; i<tableSize; i++ )
  {
    data.sine[i] = Math.sin((i/tableSize) * Math.PI * 2 )
  }
  
  // initialize PortAudio
  JsAudio.initialize()
  // setup stream options
  let streamOpts = {
    stream,
    sampleRate,
    framesPerBuffer,
    streamFlags,
    sampleFormat: formats.paFloat32,
    numInputChannels: channels,
    numOutputChannels: channels,
    callback: callback,
    userData: data
  }
  // open stream with options
  console.log(JsAudio.openDefaultStream(streamOpts))
  // start stream
  JsAudio.startStream(stream)
  // log what we're doing
  console.log(`Play for ${numSeconds} seconds.\n`)
  // stop stream when timeout fires
  setTimeout(() => {
    JsAudio.closeStream(stream)             
  }, numSeconds * 1000)
}

// Run it
callbackSine()
