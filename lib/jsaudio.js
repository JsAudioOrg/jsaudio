'use strict'

// Setup
const JsAudio = require('bindings')('jsaudio')
const jsaudio = JsAudio.initialize()

// Exports
module.exports = jsaudio
