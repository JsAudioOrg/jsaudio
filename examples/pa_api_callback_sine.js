'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

/* CALLBACK SINE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__sine_8c_source.html
*/

// Setup
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


//Create callback class that outputs a sinewave
class SineCallback {
  constructor (tableSize) {
    this.left_phase = 0
    this.right_phase = 0
    this.tableSize = tableSize
    this.sine = []
    
    // initialise sinusoidal wavetable
    for(var i=0; i<tableSize; i++ )
    {
      this.sine[i] = Math.sin((i/tableSize) * Math.PI * 2 )
    }
  }
  
  callback (input, output, frameCount) {
    var outputBufferView = new Float32Array(output)
    
    for(var i=0; i<frameCount*2; i+=2 )
    {
      outputBufferView[i] = this.sine[this.left_phase]  // left 
      outputBufferView[i+1] = this.sine[this.right_phase]  // right 
      this.left_phase += 1
      if( this.left_phase >= this.tableSize ) this.left_phase -= this.tableSize
      this.right_phase += 3 // higher pitch so we can distinguish left and right.
      if( this.right_phase >= this.tableSize ) this.right_phase -= this.tableSize
    }
  }
}

//Main function
function callbackSine () {
  // initialize stream instance
  let stream = new JsPaStream(),
      callback = new SineCallback(200)
  
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
    callback: callback.callback.bind(callback)
  }
  // open stream with options
  JsAudio.openDefaultStream(streamOpts)
  // start stream
  JsAudio.startStream(stream)
}

// Run it
callbackSine()
