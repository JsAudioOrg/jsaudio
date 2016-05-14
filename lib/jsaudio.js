'use strict'

// Setup
const JsAudioNative = require('bindings')('jsaudio')
const JsPaStream = require('bindings')('jsaudio').JsPaStream

// Exports
module.exports.JsAudioNative = JsAudioNative
module.exports.JsPaStream = JsPaStream
