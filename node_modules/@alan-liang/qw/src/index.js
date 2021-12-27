const kValue = Symbol('kValue')
const createUnderscore = (value, sync, r) => {
  const _ = sync ? f => qwi(f(value)) : f => qwi(value.then(({ value }) => ({ value: f(value) })), false)
  _.await = sync ? () => qwi(Promise.resolve(value).then(value => ({ value })), false) : () => qwi(value.then(({ value }) => value.then(value => ({ value }))), false)
  _.value = sync ? value : value.then(({ value }) => value)
  _.sideEffect = sync ? f => (f(value), r) : f => (value.then(({ value }) => f(value)), r)
  _.log = () => (sync ? console.log(value) : value.then(({ value }) => console.log(value)), r)
  return _
}
const unwrap = t => t[kValue] !== undefined ? t[kValue] : t
const unwrapAsync = t => t[kValue] !== undefined ? t[kValue].then(({ value }) => value) : Promise.resolve(t)
const qwi = (value, sync = true) => new Proxy(function () {}, {
  get: (_, p, r) =>
    p === '_' ? createUnderscore(value, sync, r) :
    p === kValue ? value :
    sync ? qwi(value[p]) :
    qwi(value.then(({ value }) => ({ value: value[p] })), false),
  apply: (_, t, a) => sync ?
    qwi(Reflect.apply(value, unwrap(t), a)) :
    qwi(Promise.all([ unwrapAsync(t), value ]).then(([ t, { value } ]) => ({ value: Reflect.apply(value, t, a) })), false),
})
const qw = exports.qw = v => qwi(v)

