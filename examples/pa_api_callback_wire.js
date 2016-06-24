'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

/* CALLBACK WIRE EXAMPLE */

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


//Create callback function that copies input to output
function callback (input, output, frameCount) {
  var outputBufferView = new Float32Array(output)
  var inputBufferView = new Float32Array(input)

  for(var i=0; i < frameCount * 2; ++i) {
    outputBufferView[i] = inputBufferView[i]
  }
}

//Main function
function callbackWire () {
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
    callback: callback
  }
  // open stream with options
  JsAudio.openDefaultStream(streamOpts)
  // start stream
  JsAudio.startStream(stream)
}

// Run it
callbackWire()
