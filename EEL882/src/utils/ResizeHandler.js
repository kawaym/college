export class ResizeHandler {
  constructor(container, camera, renderer) {
    this._container = container
    this._camera = camera
    this._renderer = renderer
    this._handler = this._onResize.bind(this)
  }

  listen() {
    window.addEventListener('resize', this._handler)
  }

  destroy() {
    window.removeEventListener('resize', this._handler)
  }

  _onResize() {
    const { clientWidth: w, clientHeight: h } = this._container
    this._camera.onResize(w, h)
    this._renderer.onResize(w, h)
  }
}
