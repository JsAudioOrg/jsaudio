'use strict'

const test = require('ava')
const JsAudio = require('./../lib/jsaudio')
const JsAudioNative = JsAudio.JsAudioNative
const JsPaStream = JsAudio.JsPaStream
let initialized = JsAudioNative.initialize()

test('JsAudioNative.initialize', (t) => {
  return t.truthy(initialized)
})
test('JsAudioNative.getVersion', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getVersion()))
})
test.todo('JsAudioNative.getErrorText')
test('JsAudioNative.getHostApiCount', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getHostApiCount()))
})
test('JsAudioNative.getDefaultHostApi', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getDefaultHostApi()))
})
test('JsAudioNative.getHostApiInfo', (t) => {
  let hostApiInfo = JsAudioNative.getHostApiInfo(
    JsAudioNative.getDefaultHostApi()
  )
  return t.truthy(
    hostApiInfo &&
    hostApiInfo.hasOwnProperty('apiIndex') &&
    hostApiInfo.apiIndex >= 0
  )
})
test.todo('JsAudioNative.getLastHostErrorInfo')
test.todo('JsAudioNative.hostApiTypeIdToHostApiIndex')
test.todo('JsAudioNative.hostApiDeviceIndexToDeviceIndex')
test('JsAudioNative.getDeviceCount', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getDeviceCount()))
})
test('JsAudioNative.getDefaultInputDevice', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getDefaultInputDevice()))
})
test('JsAudioNative.getDefaultOutputDevice', (t) => {
  return t.truthy(Number.isInteger(JsAudioNative.getDefaultOutputDevice()))
})
test('JsAudioNative.getDeviceInfo', (t) => {
  let dvcInfo = JsAudioNative.getDeviceInfo(
    JsAudioNative.getDefaultInputDevice()
  )
  return t.truthy(
    dvcInfo &&
    dvcInfo.hasOwnProperty('deviceIndex') &&
    dvcInfo.deviceIndex >= 0
  )
})
test('new JsPaStream()', (t) => {
  let stream = new JsPaStream()
  return t.truthy(stream instanceof JsPaStream)
})
test.todo('JsAudioNative.isFormatSupported')
test.todo('JsAudioNative.whyIsFormatUnsupported')
test.todo('JsAudioNative.openStream')
test.todo('JsAudioNative.openDefaultStream')
test.todo('JsAudioNative.startStream')
test.todo('JsAudioNative.stopStream')
test.todo('JsAudioNative.abortStream')
test.todo('JsAudioNative.isStreamStopped')
test.todo('JsAudioNative.getStreamWriteAvailable')
test.todo('JsAudioNative.getStreamReadAvailable')
test.todo('JsAudioNative.writeStream')
test.todo('JsAudioNative.readStream')
test('JsAudioNative.terminate', (t) => {
  return t.truthy(JsAudioNative.terminate())
})
