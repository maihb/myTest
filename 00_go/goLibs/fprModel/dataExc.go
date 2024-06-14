package main

import (
	"encoding/json"
	"libFprModule/clibs"
	"time"

	log "github.com/sirupsen/logrus"
)

const (
	RIGHT_OF_HAND         = 1   // 右手
	LEFT_OF_HAND          = 0   // 左手
	DEFAULT_WIDTH         = 640 //指纹采集宽
	DEFAULT_HEIGHT        = 640 //指纹采集高
	DEFAULT_COMPRESSRATIO = 10  //压缩比率，值越大，压缩后的数据量越少，但图像损失也越严重，推荐值为 10 或者 15。
)

func getInfo(useType, leftRight, position int, historyFeature string) (string, error) {
	if err := clibs.LsInit(); err != nil {
		return "", err
	}
	defer clibs.LsClose()

	if err := clibs.McInit(); err != nil {
		return "", err
	}
	defer clibs.McClose()

	//判断采集通道数量
	cNum, err := clibs.LsGetChannelCount()
	if err != nil {
		log.Infof("LsGetChannelCount failed: cNum[%d],err[%s]", cNum, err.Error())
		return "", err
	}
	log.Infof("LsGetChannelCount cNum=%d, leftRight=%d", cNum, leftRight)
	channel := 0
	if cNum > 1 && leftRight == RIGHT_OF_HAND {
		channel = 1
	}
	//准备采集图片
	if err := clibs.LsBeginCapture(channel); err != nil {
		return "", err
	}
	// 采集3次 采用分数最高
	w, h := DEFAULT_WIDTH, DEFAULT_HEIGHT
	maxQ := 0
	var rdata []byte
	for i := 0; i < 3; i++ {
		rdata, err = clibs.LsGetFPRawData(channel, w, h)
		if err != nil {
			return "", err
		}
		q, err := clibs.McFingerQuality(rdata, w, h)
		if err != nil {
			return "", err
		}
		if q > maxQ {
			maxQ = q
		}
		time.Sleep(300 * time.Millisecond)
	}
	//结束采集图片
	if err := clibs.LsEndCapture(channel); err != nil {
		return "", err
	}
	bmp, err := clibs.LsRawToBmp(rdata, w, h)
	if err != nil {
		log.Infof("LsRawToBmp failed: len(rdata)=%d w[%d],h[%d],err[%s]", len(rdata), w, h, err.Error())
		return "", err
	}

	wsq, err := clibs.LsBitmapToWSQ(bmp, DEFAULT_COMPRESSRATIO)
	if err != nil {
		log.Infof("LsBitmapToWSQ failed: len(wsq)=%d nCompressRatio[%d], err[%s]", len(wsq), DEFAULT_COMPRESSRATIO, err.Error())
		return "", err
	}

	//提取特征码
	fea, qa, err := clibs.LsFeatureExtractMem(bmp)
	if err != nil {
		log.Infof("LsFeatureExtractMem failed: len(fea)=%d qa[%d]  err[%s]", len(fea), qa, err.Error())
		return "", err
	}

	//返回数据
	rsp := make(map[string]interface{})

	rsp["position"] = position
	rsp["score"] = 55          // 对比分数
	rsp["collectScore"] = maxQ // 实际采集分数
	rsp["feature"] = fea
	rsp["fingerImage"] = bmp
	rsp["fingerImageCRaw"] = wsq
	rsp["sceneVideo"] = "现场视频 Base64（未实现）"
	rsp["video"] = "视频(微距) Base64（未实现）"
	rsp["realVideo"] = "实时视频 Base64（未实现）"
	str, _ := json.Marshal(rsp)
	return string(str), nil
}
