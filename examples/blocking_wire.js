'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

// Exports
module.exports = blockingWire

/* BLOCKING WIRE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__read__write__wire_8c_source.html
*/

// Setup
const numSeconds = 5
const sampleRate = 48000
const channels = 2
const framesPerBuffer = 8192
const streamFlags = 0
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

function blockingWire () {
  // initialize stream instance
  let stream = new JsPaStream()
  // stereo output buffer
  let buffer = new Float32Array(framesPerBuffer * channels)
  // log what we're doing
  console.log(
    'PortAudio Test: read from default input, write to default output\n',
    `SR = ${sampleRate}, BufSize = ${framesPerBuffer}\n`
  )
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
    numOutputChannels: channels
  }
  // open stream with options
  JsAudio.openDefaultStream(streamOpts)
  // start stream
  JsAudio.startStream(stream)
  // log what we're doing
  console.log(`Pass input to output for ${numSeconds} seconds.\n`)
  // set buffer count
  let bufferCount = ((numSeconds * sampleRate) / framesPerBuffer)
  // loop over fraames, inserting data and writing to stream
  for (let i = 0; i < bufferCount; i++) {
    JsAudio.writeStream(stream, buffer, framesPerBuffer)
    JsAudio.readStream(stream, buffer, framesPerBuffer)
  }
  JsAudio.stopStream(stream)
  console.log('Done.')
  JsAudio.closeStream(stream)
  JsAudio.terminate()
}

// Run it
blockingWire()
