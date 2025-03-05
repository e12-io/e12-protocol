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

  /*
  SELECT 
    device_id,
    evt_data->>"$.type" AS type, 
    evt_data->>"$.count" AS count, 
    evt_data->>"$.src" AS src, 
    evt_data->>"$.src_idx" AS src_idx, 
    evt_data->>"$.f" AS f, 
    evt_data->>"$.i" AS i, 
    evt_data->>"$.s" AS s, 
    evt_data->>"$.ts" AS ts_evt, 
    evt_data->>"$.v.ts" AS v_evt_ts, 
    evt_data->>"$.v.type" AS v_evt_type, 
    evt_data->>"$.v.count" AS v_evt_count, 
    evt_data->>"$.v.status" AS v_evt_status, 
    ts
  FROM timeline_evts WHERE 
  evt_data->>"$.v.ts" IS NOT NULL AND  
  evt_data->>"$.v.ts" != 0 AND 
  evt_data->>"$.f" != 0
  */
  async getTemperatureData(device_id) {
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
      "evt_data ->> \"$.f\" != 0 AND " +
      "evt_data ->> \"$.v.type\" IN(2)"; // v.type = 2 is for temperature
    sql += " AND device_id = " + mysql.escape(device_id);

    let resp = await this._fetchData(sql);
    if (resp) {
      return resp;
    }
    return null;
  }

  async getBlinkData(device_id) {
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
      "evt_data ->> \"$.v.type\" IN(1)"; // v.type = 1 is for blink
    sql += " AND device_id = " + mysql.escape(device_id);

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
