var express = require('express');
var router = express.Router();
const api = require("./demo");

router.post([
  '/temp',
  '/blink'
], function (req, res, next) {
  if (req.path === '/temp') {
    let response = api.onGetTemperatureData(req, (response) => {
      sendResponse(res, response);
    });
  } else if (req.path === '/blink') {
    let response = api.onGetBlinkData(req, (response) => {
      sendResponse(res, response);
    });
  } else {
    sendResponse(res, { error: "Invalid request" });
  }
});


function sendResponse(res, data) {
  let s = JSON.stringify(data);
  res.send(s);
}

module.exports = router;
