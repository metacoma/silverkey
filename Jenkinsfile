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
            LINUXDEPLOYQT_BUILD_DIR = "/opt/linuxdeployqt"
            TMP_FILE = "/tmp/${JOB_QT_APP}"
            ARTIFACT_SHARE_HOST_DIR = "/opt/silverkey"
            ARTIFACT_SHARE_CONTAINER_DIR = "/opt/silverkey"
            SK_ICON_PATH = "/tmp/silverkey-icon.png"
          }
          agent {
            dockerfile {
              reuseNode true
              label 'master'
              args "--privileged --cap-add SYS_PTRACE -v /opt/silverkey:/opt/silverkey"
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
            archiveArtifacts "src/${STAGE_ARTIFACT}"





            //dir("${LINUXDEPLOYQT_BUILD_DIR}") {
              sh """
              sudo mkdir --mode=777 -p "${LINUXDEPLOYQT_BUILD_DIR}"
              cd "${LINUXDEPLOYQT_BUILD_DIR}"
              pwd
              id
              ls -ltr "${LINUXDEPLOYQT_BUILD_DIR}"
              mkdir -p usr/bin usr/lib usr/share/applications usr/share/icons/hicolor/256x256/apps
              cp -v "${TMP_FILE}" "usr/bin/${JOB_QT_APP}"
              cp -v "${SK_ICON_PATH}" usr/share/icons/hicolor/256x256/apps/silverkey-qt.png
              cat<<EOF > "usr/share/applications/${JOB_QT_APP}.desktop"
[Desktop Entry]
Type=Application
Name=Silverkey
Comment=The best Qt Application Ever
Exec=${JOB_QT_APP}
Icon=${JOB_QT_APP}
Categories=Office;
EOF
              sudo /opt/Qt/5.11.0/gcc_64/bin/linuxdeployqt usr/share/applications/silverkey-qt.desktop -appimage
              sudo chown user: ${LINUXDEPLOYQT_BUILD_DIR}/Silverkey-x86_64.AppImage
              ls -ltr ${env.WORKSPACE}/
              cp -vr ${LINUXDEPLOYQT_BUILD_DIR}/Silverkey-x86_64.AppImage ${env.WORKSPACE}/
             """


            archiveArtifacts "Silverkey-x86_64.AppImage"


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
    stage('linux xfce4 funcational test') {
      environment {
        ARTIFACT_SHARE_CONTAINER_DIR = "/opt/silverkey"
      }
      agent {
        dockerfile {
          filename 'wm-xfce.Dockerfile'
          reuseNode true
          label 'master'
          args "--network=silverkey-ci-test --privileged -v /opt/silverkey:/opt/silverkey --volumes-from=silverkeyci_ci_1"
        }
      }
      steps {
        sh """
          startxfce4 &
          sleep 5
        """
        checkout scm
        dir('.') {
          sh "chmod +x /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
          sh "./test-scenario.sh /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
        }
      }
    }

    stage('linux xfce4 trusty funcational test') {
      environment {
        ARTIFACT_SHARE_CONTAINER_DIR = "/opt/silverkey"
      }
      agent {
        dockerfile {
          filename 'wm-xfce-trusty.Dockerfile'
          reuseNode true
          label 'master'
          args "--network=silverkey-ci-test --privileged -v /opt/silverkey:/opt/silverkey --volumes-from=silverkeyci_ci_1"
        }
      }
      steps {
        sh """
          startxfce4 &
          sleep 5
        """
        checkout scm
        dir('.') {
          sh "chmod +x /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
          sh "./test-scenario.sh /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
        }
      }
    }


    /*
    stage('linux fvwm2 funcational test') {
      environment {
        ARTIFACT_SHARE_CONTAINER_DIR = "/opt/silverkey"
      }
      agent {
        dockerfile {
          filename 'wm-fvwm.Dockerfile'
          reuseNode true
          label 'master'
          args "--network=silverkey-ci-test --privileged -v /opt/silverkey:/opt/silverkey --volumes-from=silverkeyci_ci_1"
        }
      }
      steps {
        sh """
          fvwm -replace &
          sleep 5
        """
        checkout scm
        dir('.') {
          sh "chmod +x /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
          sh "./test-scenario.sh /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage"
        }
      }
    }
    */

    stage('Publish latest artifacts') {
      environment {
        ARTIFACT_SHARE_CONTAINER_DIR = "/opt/silverkey"
      }
      agent {
        dockerfile {
          filename 'publish-artifacts.Dockerfile'
          reuseNode true
          label 'master'
          args "--privileged --cap-add SYS_PTRACE -v /opt/silverkey:/opt/silverkey"
        }
      }
      steps {
        sh """
          sudo cp /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/Silverkey-x86_64.AppImage ${ARTIFACT_SHARE_CONTAINER_DIR}/artifacts/silverkey-latest-linux.run

        """
        sh """
          cd /var/jenkins_home/jobs/silverkey-ui-crossplatform-build-pipeline/builds/${env.BUILD_NUMBER}/archive/
          sudo chmod +x silverkey-qt.app/Contents/MacOS/silverkey-qt
          sudo zip -r ${ARTIFACT_SHARE_CONTAINER_DIR}/artifacts/silverkey-latest-osx.zip silverkey-qt.app
        """
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
