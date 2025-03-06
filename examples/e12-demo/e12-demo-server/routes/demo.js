const db = require("./sql/db");

async function getVendorData(req, resp) {
  let err = 0;
  let errMsg = "";

  req = req.body;
  let device_id = req.device_id;
  let type = req.type;
  let from_date = req.from_date;
  let days = req.days;
  let response = await db.getVendorData(device_id, type, from_date, days);
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


module.exports.getVendorData = getVendorData;
