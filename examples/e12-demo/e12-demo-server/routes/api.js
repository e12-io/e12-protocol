var express = require('express');
var router = express.Router();
const api = require("./demo");

/**
 * Example POST request
 * POST http://localhost:8080/api/data
 * Content-Type: application/json
 * Body: 
  {
  "device_id": "207335542776556", 
  "type": 1, 
  "from_date": "2025-03-03", 
  "days": 2
  }
 */
router.post([
  '/data'
], function (req, res, next) {
  if (req.path === '/data') {
    let response = api.getVendorData(req, (response) => {
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
