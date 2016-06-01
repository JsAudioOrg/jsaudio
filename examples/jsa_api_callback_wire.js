'use strict'

// Requires
const JsAudio = require('./../lib/jsaudio')

// Exports
module.exports = callbackWire

/* BLOCKING WIRE EXAMPLE
http://portaudio.com/docs/v19-doxydocs/paex__read__write__wire_8c_source.html
*/

// Setup
const numSeconds = process.argv[2] || 5
const jsAudio = new JsAudio({sampleRate: 48000, bufferSize: 8192})

function callbackWire () {
  // log what we're doing
  console.log(
    'PortAudio Test: read from default input, write to default output\n',
    `SR = ${jsAudio.sampleRate}, BufSize = ${jsAudio.framesPerBuffer}\n`
  )
  // open stream with options
  jsAudio.openStream(null, () => {
    console.log('callback invoked')
    return 1
  })
  // start stream
  jsAudio.startStream()
  // log what we're doing
  console.log(`Pass input to output for ${numSeconds} seconds.\n`)
  // set buffer count
  // let bufferCount = ((numSeconds * jsAudio.sampleRate) / jsAudio.framesPerBuffer)
  // loop over frames, inserting data and writing to stream
  // for (let i = 0; i < bufferCount; i++) {
  //   jsAudio.writeStream()
  //   jsAudio.readStream()
  // }
  // jsAudio.stopStream()
  // console.log('Done.')
  // jsAudio.closeStream()
  // jsAudio.terminate()
}

// Run it
callbackWire()
