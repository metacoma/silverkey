
pipeline {
  agent none

  environment {
    JOB_QT_APP = "silverkey-qt"
  }


  stages {
    stage('Build') {
      parallel {
        stage('linux') {
          environment {
              APP_NAME="silverkey"
              BUILD_DIR="/tmp"
          }
          agent {
            docker {
              image "ubuntu:xenial"
              label 'master'
            }
          }
          steps {
            dir("${BUILD_DIR}") {
                dir("${BUILD_DIR}/${APP_NAME}/DEBIAN") {
                    writeFile file: 'control', text: '''
Package: silverkey
Version: 0.0-1
Section: base
Priority: optional
Architecture: amd64
Depends: rapidjson-dev
Maintainer: linux-debian@silverkey.app
Description: Silverkey app
 When you need some sunshine, just run this
 small program!
'''
                }
            }
            dir("${BUILD_DIR}/${APP_NAME}/usr/local/bin") {
              // TODO copy artifacts from build job
            }
            dir("${BUIL_DIR}") {
              dpkg-deb --build ${APP_NAME}
            }
        }
    }

}
