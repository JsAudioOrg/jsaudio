# JsAudio   [![npm version](https://badge.fury.io/js/jsaudio.svg)](http://badge.fury.io/js/jsaudio)   [![js-standard-style](https://img.shields.io/badge/code%20style-standard-brightgreen.svg?style=flat)](https://github.com/feross/standard)

## JsAudio is to Node / JS as PyAudio is to Python

### Goals

The goal of the JsAudio project is to provide a javascript implementation of PortAudio that matches the original API as closely as possible.

### Status

This is a work in progress, but currently (nearly) all PA functions are ported.
Currently PortAudio's read/write method is implemented, but the callback method is not.

Docs are in progress, as we are still refining the abstracted API. That being said,
the `JsAudioNative` api directly follows [PortAudio's API](http://portaudio.com/docs/v19-doxydocs/api_overview.html). You can access this by
requiring like so `require('jsaudio').JsAudioNative`

Until the  abstracted API is finalized you can check out the  [examples](https://github.com/JsAudioOrg/jsaudio/tree/master/examples) directory to see usage at it's current state. Also, feel free to look at the JsAudio class in [lib/jsaudio.js](https://github.com/JsAudioOrg/jsaudio/blob/master/lib/jsaudio.js)
which serves as the abstraction API.

### Contributing
1. Fork it
2. Create your feature branch (git checkout -b my-new-feature)
3. Ensure code style is consistent (js-standard for js)
4. Commit your changes (git commit -am 'Added some feature')
5. Push to the branch (git push origin my-new-feature)
6. Create new Pull Request
