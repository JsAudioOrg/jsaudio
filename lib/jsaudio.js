'use strict'

// Setup
const JsAudioNative = require('bindings')('jsaudio')
const JsPaStream = require('bindings')('jsaudio').JsPaStream
const util = require('util')
const Event = require('events').EventEmitter
const Transform = require('readable-stream/transform')
const _ = require('./helpers')()

// Exports
module.exports = JsAudio
module.exports.JsPaStream = JsPaStream
module.exports.JsAudioNative = JsAudioNative

// JsAudio class
function JsAudio (opts) {
  let self = this
  // Inherit all JsAudioNative methods through try catch pseudo proxy
  Object.keys(JsAudioNative).forEach((k) => {
    self[k] = (...args) => self.tryCatch(k, ...args)
  })
  // _.merge(self, JsAudioNative, false)
  _.merge(self, opts, false)
  return self
}

// JsAudio inherits from EventEmitter
util.inherits(JsAudio, Event)
util.inherits(JsAudio, Transform)

// Return new instance of JsAudio
JsAudio.new = (opts) => new JsAudio(opts)

// Try / Catch helper function
JsAudio.prototype.tryCatch = function (fnName, ...args) {
  let self = this
  setTimeout(() => {
    let out
    try {
      out = JsAudioNative[fnName](...args)
    } catch (e) {
      if (Object.keys(self._events).indexOf('error') !== -1) {
        return self.emit('error', e)
      }
      throw (e)
    }
    self.emit(`${_.dasherize(fnName)}-done`, out)
    return self
  }, 0)
  return self
}
