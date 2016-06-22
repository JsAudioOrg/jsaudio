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
    callback: ()=> {console.log('Callback')}
  }
  // open stream with options
  console.log(JsAudio.openDefaultStream(streamOpts));
  
  JsAudio.startStream(stream);
}

// Run it
blockingSine()
