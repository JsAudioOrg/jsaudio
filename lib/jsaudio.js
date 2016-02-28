'use strict'

// Setup
const JsAudioNative = require('bindings')('jsaudio')
const util = require('util')
const Event = require('events').EventEmitter
const _ = require('./helpers')()

// Exports
module.exports = JsAudio

// JsAudio class
function JsAudio (opts) {
  let self = this
  // Inherit all JsAudioNative methods, add error and success events
  Object.keys(JsAudioNative).forEach((k) => {
    let tmpFunc = function (...args) {
      let out
      try {
        out = JsAudioNative[k](...args)
      } catch (e) {
        if (Object.keys(self._events).indexOf('error') !== -1) {
          return self.emit('error', e)
        }
        throw (e)
      }
      self.emit(`${_.dasherize(k)}-done`, out)
      return self
    }
    self[k] = (...args) => setTimeout(() => tmpFunc(...args), 0)
  })
  _.merge(self, opts, false)
  return self
}

// JsAudio inherits from EventEmitter
util.inherits(JsAudio, Event)

// Return new instance of JsAudio
JsAudio.new = (opts) => new JsAudio(opts)
