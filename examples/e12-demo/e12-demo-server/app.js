var createError = require('http-errors')
var express = require('express')
const cors = require('cors')
var path = require('path')
var cookieParser = require('cookie-parser')
var serveStatic = require('serve-static')
const db = require('./routes/sql/db')

var app = express()

app.set('trust proxy', 1) // trust first proxy
app.use(cookieParser());
app.use(function (req, res, next) {
  res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS');
  res.setHeader('Access-Control-Allow-Headers', 'X-Requested-With,content-type');
  res.setHeader('Access-Control-Allow-Credentials', true);
  // Pass to next layer of middleware
  next();
});

var whitelist = JSON.parse(process.env.CROSS_ORIGIN_DOMAINS);
var corsOptions = {
  credentials: true,
  methods: ["POST", "PUT", "GET", "OPTIONS", "HEAD", "DELETE"],
  origin: function (origin, callback) {
    if (whitelist.indexOf(origin) !== -1 || !origin) {
      callback(null, true)
    } else {
      callback(new Error('Not allowed by CORS'))
    }
  }
}

// use this when accessing from web app
// app.use(cors(corsOptions));

// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'pug');

app.use(express.json());
app.use(express.urlencoded({
  extended: true
}));

var apiRouter = require('./routes/api');
app.use(serveStatic('./public', { acceptRanges: true }));
app.use('/api', apiRouter);

// catch 404 and forward to error handler
app.use(function (req, res, next) {
  next(createError(404));
});

// error handler
app.use(function (err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

module.exports = app;
