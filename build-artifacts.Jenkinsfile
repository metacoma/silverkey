
pipeline {
  agent none

  environment {
    JOB_QT_APP = "silverkey-qt"
    TMP_DIR = "/tmp/silverkey-build-artifacts"
  }


  stages {
    stage('Build') {
      parallel {
        stage('linux') {
          environment {
              APP_NAME="silverkey"
          }
          agent {
            docker {
              image "ubuntu:xenial"
              label 'master'
            }
          }

          steps {

            dir('/tmp') {
                dir("${APP_NAME}") {
                    writeFile file: 'DEBIAN', text: '''
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


                script {
                    step ([$class: 'CopyArtifact',
                    projectName: 'silverkey-ui-crossplatform-build-pipeline',
                    filter: "src/silverkey-qt-linux-x64_86",
                    target: "/tmp/${APP_NAME}/usr/local/bin/"]);
                }

                sh "dpkg-deb --build ${APP_NAME} ."
          }
        }
      }
   }
  }
}
