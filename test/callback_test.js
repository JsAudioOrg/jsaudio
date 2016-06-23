'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

/* BLOCKING SINE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__write__sine_8c_source.html
*/

// Setup
const sampleRate = 48000
const channels = 2
const framesPerBuffer = 2048
const streamFlags = 0
const tableSize = 400
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

var left_phase = 0
var right_phase = 0
var sine = []

// initialise sinusoidal wavetable
for(var i=0; i<tableSize; i++ )
{
  sine[i] = Math.sin((i/tableSize) * Math.PI * 2 )
}

function SineCallback(input, output, frameCount) {
  var outputBufferView = new Float32Array(output);
  
  for(var i=0; i<frameCount*2; i+=2 )
  {
    outputBufferView[i] = sine[left_phase]  /* left */
    outputBufferView[i+1] = sine[right_phase]  /* right */
    left_phase += 1
    if( left_phase >= tableSize ) left_phase -= tableSize;
    right_phase += 3 /* higher pitch so we can distinguish left and right. */
    if( right_phase >= tableSize ) right_phase -= tableSize;
  }
}




//Main function
function blockingSine () {
  // initialize stream instance
  let stream = new JsPaStream()
  
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
    callback: SineCallback
  }
  // open stream with options
  console.log(JsAudio.openDefaultStream(streamOpts))
  
  JsAudio.startStream(stream);
}

// Run it
blockingSine()
