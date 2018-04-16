pipeline {
    agent {
        docker { image 'ubuntu:xenial' }
    }
    stages {
        stage('Build') {
            steps {
                sh 'lsb_release -a'
            }
        }
    }
}
