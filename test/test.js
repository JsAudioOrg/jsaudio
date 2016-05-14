'use strict'

const test = require('ava')
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
const JsPaStream = JsAudio.JsPaStream
let initialized = JsAudio.initialize()

test('JsAudio.initialize', (t) => {
  return t.truthy(initialized)
})
test('JsAudio.getVersion', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getVersion()))
})
test('JsAudio.getHostApiCount', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getHostApiCount()))
})
test('JsAudio.getDefaultHostApi', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getDefaultHostApi()))
})
test('JsAudio.getHostApiInfo', (t) => {
  let hostApiInfo = JsAudio.getHostApiInfo(JsAudio.getDefaultHostApi())
  return t.truthy(
    hostApiInfo &&
    hostApiInfo.hasOwnProperty('apiIndex') &&
    hostApiInfo.apiIndex >= 0
  )
})
test('JsAudio.getDeviceCount', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getDeviceCount()))
})
test('JsAudio.getDefaultInputDevice', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getDefaultInputDevice()))
})
test('JsAudio.getDefaultOutputDevice', (t) => {
  return t.truthy(Number.isInteger(JsAudio.getDefaultOutputDevice()))
})
test('JsAudio.getDeviceInfo', (t) => {
  let dvcInfo = JsAudio.getDeviceInfo(JsAudio.getDefaultInputDevice())
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
test.todo('JsAudio.openStream')
test.todo('JsAudio.openDefaultStream')
test.todo('JsAudio.startStream')
test.todo('JsAudio.getStreamWriteAvailable')
test.todo('JsAudio.getStreamReadAvailable')
test.todo('JsAudio.writeStream')
test.todo('JsAudio.readStream')
test('JsAudio.terminate', (t) => {
  return t.truthy(JsAudio.terminate())
})
