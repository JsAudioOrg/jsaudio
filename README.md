# JsAudio

## JsAudio is to Node / JS as PyAudio is to Python

### Goals

The goal of the JsAudio project is to provide a javascript implementation of PortAudio that matches the original API as closely as possible.

### Status

This is a work in progress and is in no way ready to use in any fashion at this time.

The javascript API is far from finalized, though it will closely follow PortAudio's API and should look similar to PyAudio in usage.

Currently the utility functions have been implemented in NAN and are exposed in a very rough javascript API. 

The stream handling is still in development and as such the library is not ready for usage until that is fully implemented.

### Contributing
Currently the javascript is written in [JS Standard Style](https://github.com/feross/standard). That is [my](https://github.com/musocrat) preference, but it is open for discussion. I only ask that we maintain consistent styling for the project.

1. Fork it
2. Create your feature branch (git checkout -b my-new-feature)
3. Commit your changes (git commit -am 'Added some feature')
4. Push to the branch (git push origin my-new-feature)
5. Create new Pull Request
