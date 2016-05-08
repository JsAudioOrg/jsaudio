'use strict'

const test = require('ava')
const JsAudioExports = require('./../lib/jsaudio')
const JsAudio = JsAudioExports.JsAudioNative
// const JsPaStream = JsAudio.JsPaStream

test('JsAudio.initialize', (t) => {
  return t.truthy(JsAudio.initialize(), 'successfully initialized')
})
test.todo('JsAudio.terminate')
test.todo('JsAudio.getVersion')
test.todo('JsAudio.getHostApiCount')
test.todo('JsAudio.getDefaultHostApi')
test.todo('JsAudio.getHostApiInfo')
test.todo('JsAudio.getDeviceCount')
test.todo('JsAudio.getDefaultInputDevice')
test.todo('JsAudio.getDefaultOutputDevice')
test.todo('JsAudio.getDeviceInfo')
test.todo('JsAudio.openStream')
test.todo('JsAudio.openDefaultStream')
test.todo('JsAudio.startStream')
test.todo('JsAudio.getStreamWriteAvailable')
test.todo('JsAudio.getStreamReadAvailable')
test.todo('JsAudio.writeStream')
test.todo('JsAudio.readStream')
