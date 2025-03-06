const mysql = require("mysql2");

var session = require('express-session');
var MySQLStore = require('express-mysql-session')(session);

class e12demoDb {
  constructor() {
    let connectionParams = this._getDBConnParams();
    this.pool = mysql.createPool(connectionParams);
    this._fetchData = this._fetchData.bind(this);
  }

  _getDBConnParams() {
    return {
      connectionLimit: 10,
      host: process.env.DB_HOST,
      port: process.env.DB_PORT,
      user: process.env.DB_USER,
      password: process.env.DB_PASS,
      database: process.env.DB_NAME,
    };
  }

  initSession() {
  }

  async _fetchData(statement, options = {
    connection: null,
  }) {
    let pool = this.pool;
    let sql = statement;
    let promise = new Promise(function (resolve, reject) {
      let query_func = (options.connection) ? options.connection.query.bind(options.connection) : pool.query.bind(pool);
      query_func(sql, function (error, results, fields) {
        if (error) reject(error);
        else {
          results = JSON.parse(JSON.stringify(results));
          resolve(results);
        }
      });
    });
    return promise;
  }

  /**
   * 
   * @param {*} device_id 
   * @param {*} type 
   * @param {*} from_date 
   * @param {*} days 
   * @returns 
   */
  async getVendorData(device_id, type, from_date, days) {
    //  no device_id or type, return null
    if (device_id == null || type == null) {
      return null;
    }

    // no from_date, set to today
    if (from_date == null) {
      from_date = new Date();
      from_date.setHours(0, 0, 0, 0);
      from_date = from_date.toISOString().split('T')[0];
    }

    // no days, set to 1
    if (days == null) {
      days = 1;
    }

    let sql = "  SELECT device_id," +
      "evt_data ->> \"$.type\" AS type," +
      "evt_data ->> \"$.count\" AS count," +
      "evt_data ->> \"$.src\" AS src," +
      "evt_data ->> \"$.src_idx\" AS src_idx," +
      "evt_data ->> \"$.f\" AS f," +
      "evt_data ->> \"$.i\" AS i," +
      "evt_data ->> \"$.s\" AS s," +
      "evt_data ->> \"$.ts\" AS ts_evt," +
      "evt_data ->> \"$.v.ts\" AS v_evt_ts," +
      "evt_data ->> \"$.v.type\" AS v_evt_type," +
      "evt_data ->> \"$.v.count\" AS v_evt_count," +
      "evt_data ->> \"$.v.status\" AS v_evt_status," +
      "ts " +
      "FROM timeline_evts WHERE " +
      "evt_data ->> \"$.v.ts\" IS NOT NULL AND " +
      "evt_data ->> \"$.v.ts\" != 0 AND " +
      "evt_data ->> \"$.ts\" / 1000000 >= UNIX_TIMESTAMP(" + mysql.escape(from_date) + ") AND " +
      "evt_data ->> \"$.ts\" / 1000000 < UNIX_TIMESTAMP(DATE_ADD(" + mysql.escape(from_date) + ", INTERVAL " + days + " DAY)) AND " +
      "evt_data ->> \"$.v.type\" IN(" + type + ") AND " +
      "device_id = " + mysql.escape(device_id) +
      " ORDER BY ts_evt DESC";

    let resp = await this._fetchData(sql);
    if (resp) {
      return resp;
    }
    return null;
  }

}

let db = null;
function getDB() {
  if (db == null) {
    db = new e12demoDb();
  }
  return db;
}

module.exports = getDB();
