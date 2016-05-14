'use strict'

// Setup
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

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
let streamOpts = {
  input: {
    device: 1,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.003
  },
  output: {
    device: 3,
    channelCount: 2,
    sampleFormat: formats.paFloat32,
    suggestedLatency: 0.003
  },
  sampleRate: 48000,
  framesPerBuffer: 0,
  streamFlags: 0
}

let stream = new JsPaStream()
streamOpts.stream = stream
// Exports
module.exports = JsAudio

let buffer
setTimeout(() => {
  let framesToWrite = 0
  let framesToRead = 0
  // let phase = 0
  // let phaseIncrement = (2 * Math.PI * 80) / 48000.0
  let readBuffer = new Float32Array()
  while (1) {
    framesToRead = JsAudio.getStreamReadAvailable(stream)
    if (framesToRead) {
      let tempBuffer = new Float32Array(framesToRead * 2)
      JsAudio.readStream(stream, tempBuffer)

      let copyBuffer = new Float32Array(readBuffer.length + tempBuffer.length)
      copyBuffer.set(readBuffer)
      copyBuffer.set(tempBuffer, readBuffer.length)
      readBuffer = new Float32Array(copyBuffer)
    }
    framesToWrite = JsAudio.getStreamWriteAvailable(stream)
    if (framesToWrite && readBuffer.length > 0) {
      buffer = new Float32Array(readBuffer.buffer, 0, framesToWrite * 2)

      JsAudio.writeStream(stream, buffer)

      readBuffer = new Float32Array(
        readBuffer.buffer,
        framesToWrite * 2 * Float32Array.BYTES_PER_ELEMENT
      )
    }
  }
}, 1000)
