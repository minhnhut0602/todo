/// <reference path="../scss.d.ts" />
/// <reference path="../html.d.ts" />

import { TARGET } from './util'

import '../src/scss/app.scss'
import App from './App.html'
new App({ target: document.getElementById(TARGET) })
