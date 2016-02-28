'use strict'

// Exports
module.exports = () => new Helpers()

function Helpers () {}

// Merge object B into A, return A
Helpers.prototype.merge = (a, b, preferA) => {
  if (typeof a !== 'object') a = {}
  if (!b) return a
  if (typeof b !== 'object') b = {}
  Object.keys(b).forEach((k) => { a[k] = preferA ? a[k] || b[k] : b[k] })
  return a
}

// String to lowercase dashed
Helpers.prototype.dasherize = (str) => {
  return str.replace(/\.?([A-Z]+)/g, (x, y) => (' ' + y)).trim()
    .replace(/\s+|[,-;\[\]\\\/]/g, '-').toLowerCase()
}
