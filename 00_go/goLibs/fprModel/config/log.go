package config

import (
	"os"
	"path/filepath"

	nested "github.com/antonfisher/nested-logrus-formatter"
	log "github.com/sirupsen/logrus"
	"gopkg.in/natefinch/lumberjack.v2"
)

var (
	_logName         = "log/fprModule.log"
	_timeStampFormat = "2006-01-02 15:04:05.000"
)

func init() {
	directory, err := filepath.Abs(filepath.Dir(os.Args[0])) //get the current working directory
	if err == nil {
		_logName = directory + "/" + _logName
	}

	log.SetFormatter(&nested.Formatter{
		NoColors:        true,
		TimestampFormat: _timeStampFormat,
	})
	log.SetOutput(&lumberjack.Logger{
		Filename:   _logName,
		MaxSize:    20, //单文件大小，单位：MB
		MaxBackups: 5,
		LocalTime:  true,
		Compress:   true, //是否压缩，解压命令用gunzip
	})
}
