'use strict'

// Setup
const JsAudioNative = require('bindings')('jsaudio')
const JsPaStream = require('bindings')('jsaudio').JsPaStream
// define PortAudio formats
const formats = {
  paFloat32: 1,
  paInt32: 2,
  paInt16: 8,
  paInt8: 16,
  paUInt8: 32,
  paCustomFormat: 65536,
  paNonInterleaved: 2147483648
}
// provide convenient aliases for formats (paFloat32 -> float32)
Object.keys(formats).forEach((k) => {
  formats[k.substr(2).toLowerCase()] = formats[k]
})
// provide format through constructor (JsAudio.float32)
Object.keys(formats).forEach((k) => JsAudio[k] = formats[k])

// Exports
module.exports = JsAudio
module.exports.JsAudioNative = JsAudioNative
module.exports.JsPaStream = JsPaStream

// Helpers
const readWriteStreamArgs = (ref, stream, buffer, framesPerBuffer) => {
  if (!framesPerBuffer && Number.isInteger(buffer)) {
    framesPerBuffer = buffer
    buffer = null
  }
  framesPerBuffer = framesPerBuffer || ref.framesPerBuffer
  buffer = buffer || ref.buffer || ref.getBuffer()
  stream = stream || ref.stream || ref.getStream()
  return {stream, buffer, framesPerBuffer}
}

// JsAudio constructor
function JsAudio (opts) {
  let self = this
  let defaultInputDevice, defaultOutputDevice, inputInfo, outputInfo
  JsAudioNative.initialize()
  opts = opts && opts instanceof Object ? opts : {}
  opts.format = opts.format || formats.float32
  opts.streamFlags = opts.streamFlags || opts.flags || 0
  opts.channels = opts.channels || opts.channelCount || 2
  opts.sampleRate = opts.sampleRate || opts.samplerate || 44100
  opts.framesPerBuffer = opts.framesPerBuffer || opts.bufferFrames || 1024
  if (!(opts.inputParams || opts.input)) {
    defaultInputDevice = JsAudioNative.getDefaultInputDevice()
    inputInfo = JsAudioNative.getDeviceInfo(defaultInputDevice)
  }
  if (!(opts.outputParams || opts.output)) {
    defaultOutputDevice = JsAudioNative.getDefaultOutputDevice()
    outputInfo = JsAudioNative.getDeviceInfo(defaultOutputDevice)
  }
  opts.inputParams = opts.inputParams || opts.input || {
    device: defaultInputDevice,
    channelCount: opts.channels,
    sampleFormat: opts.format,
    suggestedLatency: inputInfo.defaultLowOutputLatency || 0.003
  }
  opts.outputParams = opts.outputParams || opts.output || {
    device: defaultOutputDevice,
    channelCount: opts.channels,
    sampleFormat: opts.format,
    suggestedLatency: outputInfo.defaultLowOutputLatency || 0.003
  }
  Object.keys(opts).forEach((k) => self[k] = opts[k])
  // provide formats through instance (jsAudio.float32) for convenience
  Object.keys(formats).forEach((k) => self[k] = formats[k])
  // extend JsAudioNative onto instance
  Object.keys(JsAudioNative).forEach(
    (k) => self[k] = self[k] || JsAudioNative[k]
  )
  return self
}

// Create new instance
JsAudio.new = (opts) => new JsAudio(opts)

// Get buffer of specified format
JsAudio.prototype.getBuffer = function (format, framesPerBuffer, channels) {
  let self = this
  channels = channels || self.channels
  framesPerBuffer = framesPerBuffer || self.framesPerBuffer
  format = format || self.format
  format = Number.isInteger(format)
    ? (Object.keys(formats).filter((f) => formats[f] === format) || [])[0]
    : format
  let size = framesPerBuffer * channels
  if (format.match(/^pa/)) format = format.substr(2)
  else format = format.charAt(0).toUpperCase() + format.substr(1)
  self.buffer = typeof global[`${format}Array`] === 'function'
    ? new global[`${format}Array`](size) : new Float32Array(size)
  return self.buffer
}

// Get new stream object
JsAudio.prototype.getStream = function (format) {
  let self = this
  self.stream = new JsPaStream()
  return self.stream
}

// Override inherited openStream method
JsAudio.prototype.openStream = function (opts) {
  let self = this
  opts = opts || {
    stream: self.stream || self.getStream(),
    sampleRate: self.sampleRate,
    framesPerBuffer: self.framesPerBuffer,
    streamFlags: self.streamFlags,
    sampleFormat: self.format,
    input: self.inputParams,
    output: self.outputParams
  }
  JsAudioNative.openStream(opts)
  return self
}

// Override inherited openDefaultStream method
JsAudio.prototype.openDefaultStream = function (opts) {
  let self = this
  opts = opts || {
    stream: self.stream || self.getStream(),
    sampleRate: self.sampleRate,
    framesPerBuffer: self.framesPerBuffer,
    streamFlags: self.streamFlags,
    sampleFormat: self.format,
    numInputChannels: self.channels,
    numOutputChannels: self.channels
  }
  JsAudioNative.openDefaultStream(opts)
  return self
}

// Override inherited startStream method
JsAudio.prototype.startStream = function (stream) {
  let self = this
  JsAudioNative.startStream(stream || self.stream)
  return self
}

// Override inherited stopStream method
JsAudio.prototype.stopStream = function (stream) {
  let self = this
  JsAudioNative.stopStream(stream || self.stream)
  return self
}

// Override inherited closeStream method
JsAudio.prototype.closeStream = function (stream) {
  let self = this
  JsAudioNative.closeStream(stream || self.stream)
  return self
}

// Override inherited writeStream method
JsAudio.prototype.writeStream = function (stream, buffer, framesPerBuffer) {
  let self = this
  let args = readWriteStreamArgs(self, stream, buffer, framesPerBuffer)
  JsAudioNative.writeStream(
    args.stream, args.buffer, args.framesPerBuffer, self.channels
  )
  return self
}

// Override inherited readStream method
JsAudio.prototype.readStream = function (stream, buffer, framesPerBuffer) {
  let self = this
  let args = readWriteStreamArgs(self, stream, buffer, framesPerBuffer)
  JsAudioNative.readStream(
    args.stream, args.buffer, args.framesPerBuffer, self.channels
  )
  return self
}

// Get device info for all host APIs as array
JsAudio.prototype.getHostApiList = function () {
  let self = this
  let length = JsAudioNative.getHostApiCount()
  let refAry = Array.apply(null, {length}).map(Number.call, Number)
  self.hosts = refAry.map((d) => JsAudioNative.getHostApiInfo(d))
  return self.hosts
}

// Get device info for all devices as array
JsAudio.prototype.getDeviceList = function () {
  let self = this
  let length = JsAudioNative.getDeviceCount()
  let refAry = Array.apply(null, {length}).map(Number.call, Number)
  self.devices = refAry.map((d) => JsAudioNative.getDeviceInfo(d))
  return self.devices
}
