'use strict'

// Requires
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudioExports.JsPaStream

// Exports
module.exports = blockingSine

/* BLOCKING SINE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__write__sine_8c_source.html
*/

// Setup
const numPlays = 8
const numSeconds = 1
const sampleRate = 48000
const channels = 2
const framesPerBuffer = 8192
const streamFlags = 0
const pi = 3.14159265
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
  // stereo output buffer
  let buffer = new Float32Array(framesPerBuffer * channels)
  // sine wavetable
  let sine = new Float32Array(tableSize)
  // setup phase
  let phase = {left: 0, right: 0}
  // increment right to higher pitch so we can distinguish left and right
  let increment = {left: 1, right: 3}
  // log what we're doing
  console.log(
    'PortAudio Test: output sine wave\n',
    `SR = ${sampleRate}, BufSize = ${framesPerBuffer}\n`
  )
  // initialize sinusoidal wavetable
  for (let i = 0; i < tableSize; i++) {
    sine[i] = Math.sin((i / tableSize) * pi * 2)
  }
  // initialize PortAudio
  JsAudio.initialize()
  // setup stream options
  let defaultInputDevice = JsAudio.getDefaultInputDevice()
  let defaultOutputDevice = JsAudio.getDefaultOutputDevice()
  let inputInfo = JsAudio.getDeviceInfo(defaultInputDevice)
  let outputInfo = JsAudio.getDeviceInfo(defaultOutputDevice)
  let streamOpts = {
    stream,
    sampleRate,
    framesPerBuffer,
    streamFlags,
    input: {
      device: defaultInputDevice,
      channelCount: channels,
      sampleFormat: formats.paFloat32,
      suggestedLatency: inputInfo.defaultLowOutputLatency || 0.003
    },
    output: {
      device: defaultOutputDevice,
      channelCount: channels,
      sampleFormat: formats.paFloat32,
      suggestedLatency: outputInfo.defaultLowOutputLatency || 0.003
    }
  }
  // open stream with options
  JsAudio.openStream(streamOpts)
  // log what we're doing
  console.log(`Play ${numPlays} times, higher each time.\n`)
  // start playing
  for (let k = 0; k < numPlays; ++k) {
    setTimeout(() => {
      // start stream
      JsAudio.startStream(stream)
      // log what we're doing
      console.log(`Play for ${numSeconds} seconds.\n`)
      // set buffer count
      let bufferCount = ((numSeconds * sampleRate) / framesPerBuffer)

      for (let i = 0; i < bufferCount; i++) {
        for (let j = 0; j < framesPerBuffer; j++) {
          buffer[j * i] = sine[phase.left] * 0.05
          buffer[j * i + 1] = sine[phase.right] * 0.05
          phase.left += increment.left
          if (phase.left >= tableSize) phase.left -= tableSize
          phase.right += increment.right
          if (phase.right >= tableSize) phase.right -= tableSize
        }
        JsAudio.writeStream(stream, buffer, framesPerBuffer)
      }
      JsAudio.stopStream(stream)

      if (k < (numPlays / 2) - 1) {
        increment.left++
        increment.right++
      } else {
        increment.left--
        increment.right--
      }
      // if we're done terminate PortAudio
      if (k === (numPlays - 1)) {
        console.log('Done.')
        JsAudio.terminate()
      }
    },
    1000)
  }
}

// Run it
blockingSine()
