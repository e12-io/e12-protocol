const db = require("./sql/db");

async function onGetTemperatureData(req, resp) {
  let err = 0;
  let errMsg = "";

  req = req.body;
  let deviceId = req.deviceId;
  let response = await db.getTemperatureData(deviceId);
  if (response == null) {
  }

  let r = {
    status: 200,
    body: {
      status: err,
      data: (!err) ? response : null,
    },
  };
  resp(r);
}

async function onGetBlinkData(req, resp) {
  let err = 0;
  let errMsg = "";

  req = req.body;
  let deviceId = req.deviceId;
  let response = await db.getBlinkData(deviceId);
  if (response == null) {
  }

  let r = {
    status: 200,
    body: {
      status: err,
      data: (!err) ? response : null,
    },
  };
  resp(r);
}

module.exports.onGetTemperatureData = onGetTemperatureData;
module.exports.onGetBlinkData = onGetBlinkData;
