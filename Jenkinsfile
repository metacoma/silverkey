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
            STAGE_ARCH = "x64_86"
            STAGE_OS = "linux"
            STAGE_ARTIFACT = "${JOB_QT_APP}-${STAGE_OS}-${STAGE_ARCH}"
            LINUXDEPLOYQT_BUILD_DIR = "/tmp/linuxdeployqt"
            TMP_FILE = "/tmp/${JOB_QT_APP}"
            SK_ICON_PATH = "/tmp/silverkey-icon.png"
          }
          agent {
            dockerfile {
              reuseNode true
              label 'master'
              args '--privileged'
            }
          }
          steps {
            step([$class: 'WsCleanup'])
            echo "---- PR build test 8888"
            checkout scm
            dir('src') {
              sh 'qmake'
              sh 'make -j4'
              sh "chmod +x ${JOB_QT_APP}"
              sh "cp -v ${JOB_QT_APP} ${TMP_FILE}"
              sh "mv -v ${JOB_QT_APP} ${STAGE_ARTIFACT}"
            }

            sh "mkdir -p ${LINUXDEPLOYQT_BUILD_DIR}"
            sh "cd ${LINUXDEPLOYQT_BUILD_DIR}"
            //dir("${LINUXDEPLOYQT_BUILD_DIR}") {
              sh "mkdir -p usr/bin usr/lib usr/share/applications usr/share/icons/hicolor/256x256/apps"
              sh "cp -v ${TMP_FILE} usr/bin/${JOB_QT_APP}"
              sh "cp -v ${SK_ICON_PATH} usr/share/icons/hicolor/256x256/apps/silverkey-qt.png"
              writeFile file: "usr/share/applications/${JOB_QT_APP}.desktop", text: """[Desktop Entry]
Type=Application
Name=Silverkey
Comment=The best Qt Application Ever
Exec=${JOB_QT_APP}
Icon=${JOB_QT_APP}
Categories=Office;
"""
              sh "linuxdeployqt usr/share/applications/silverkey-qt.desktop -appimage"
            //}

            archiveArtifacts "${LINUXDEPLOYQT_BUILD_DIR}/Silverkey-x86_64.AppImage"
            archiveArtifacts "src/${STAGE_ARTIFACT}"
          }
        }
        /*
        stage('qt-5.9-linux-static') {
          environment {
            STAGE_ARCH = "x64_86"
            STAGE_OS = "linux"
            STAGE_ARTIFACT = "${JOB_QT_APP}-${STAGE_OS}-static-${STAGE_ARCH}"
          }
          agent {
            dockerfile {
              filename 'qt_5_9_linux_static.Dockerfile'
              reuseNode true
              label 'master'
            }
          }
          steps {
            step([$class: 'WsCleanup'])
            checkout scm
            dir('src') {
              sh 'qmake -profile release'
              sh 'make -j4'
              sh "mv -v ${JOB_QT_APP} ${STAGE_ARTIFACT}"
            }
            archiveArtifacts "src/${STAGE_ARTIFACT}"
          }
        }
        */
        stage('osx') {
          environment {
            STAGE_ARTIFACT = "${JOB_QT_APP}.app/**"
          }
          agent {
            label 'mac-slave'
          }
          steps {
            step([$class: 'WsCleanup'])
            echo "---- PR build test 8888"
            checkout scm
            dir('src') {
              sh '/usr/local/Cellar/qt/5.10.1/bin/qmake'
              sh 'make -j4'
              sh '/usr/local/Cellar/qt/5.10.1/bin/macdeployqt ${JOB_QT_APP}.app'

              archiveArtifacts "${STAGE_ARTIFACT}"
            }
          }
        }
      }
    }
  }
  post {
    success {
      script {
        // CHANGE_ID is set only for pull requests, so it is safe to access the pullRequest global variable
        if (env.CHANGE_ID) {
          pullRequest.addLabel('Build success')
        }
      }
    }
  }
}
