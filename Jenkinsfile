
/*
** Variables.
*/
def serie = '21.04'
def maintenanceBranch = "${serie}.x"
def qaBranch = "dev-${serie}.x"

if (env.BRANCH_NAME.startsWith('release-')) {
  env.BUILD = 'RELEASE'
} else if ((env.BRANCH_NAME == 'master') || (env.BRANCH_NAME == maintenanceBranch)) {
  env.BUILD = 'REFERENCE'
} else if ((env.BRANCH_NAME == 'develop') || (env.BRANCH_NAME == qaBranch)) {
  env.BUILD = 'QA'
} else {
  env.BUILD = 'CI'
}

/*
** Pipeline code.
*/
stage('Deliver sources') {
  node("C++") {
    sh 'setup_centreon_build.sh'
    dir('centreon-broker') {
      checkout scm
    }
    sh "./centreon-build/jobs/broker/${serie}/mon-broker-source.sh"
    source = readProperties file: 'source.properties'
    env.VERSION = "${source.VERSION}"
    env.RELEASE = "${source.RELEASE}"
  }
}

try {
  stage('Build // Unit tests // Packaging') {
    parallel 'build centos7': {
      node("C++") {
        sh 'setup_centreon_build.sh'
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-unittest.sh centos7"
        step([
          $class: 'XUnitBuilder',
          thresholds: [
            [$class: 'FailedThreshold', failureThreshold: '0'],
            [$class: 'SkippedThreshold', failureThreshold: '0']
          ],
          tools: [[$class: 'GoogleTestType', pattern: 'ut.xml']]
        ])
        // Run sonarQube analysis
        withSonarQubeEnv('SonarQubeDev') {
          sh "./centreon-build/jobs/broker/${serie}/mon-broker-analysis.sh"
        }
      }
    },
    'packaging centos7': {
      node("C++") {
        sh 'setup_centreon_build.sh'
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-package.sh centos7"
        stash name: 'el7-rpms', includes: "output/x86_64/*.rpm"
        archiveArtifacts artifacts: "output/x86_64/*.rpm"
      }
    },
    'build alma8': {
      node("C++") {
        checkoutCentreonBuild()
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-unittest.sh alma8"
        step([
          $class: 'XUnitBuilder',
          thresholds: [
            [$class: 'FailedThreshold', failureThreshold: '0'],
            [$class: 'SkippedThreshold', failureThreshold: '0']
          ],
          tools: [[$class: 'GoogleTestType', pattern: 'ut.xml']]
        ])
      }
    },
    'packaging alma8': {
      node("C++") {
        checkoutCentreonBuild()
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-package.sh alma8"
        stash name: 'alma8-rpms', includes: "output/x86_64/*.rpm"
        archiveArtifacts artifacts: "output/x86_64/*.rpm"
      }
//    },
//    'build debian10': {
//      node("C++") {
//        sh 'setup_centreon_build.sh'
//        sh "./centreon-build/jobs/broker/${serie}/mon-broker-unittest.sh debian10"
//        step([
//          $class: 'XUnitBuilder',
//          thresholds: [
//            [$class: 'FailedThreshold', failureThreshold: '0'],
//            [$class: 'SkippedThreshold', failureThreshold: '0']
//          ],
//          tools: [[$class: 'GoogleTestType', pattern: 'ut.xml']]
//        ])
//      }
//    },
//    'packaging debian10': {
//      node("C++") {
//        sh 'setup_centreon_build.sh'
//        sh "./centreon-build/jobs/broker/${serie}/mon-broker-package.sh debian10"
//      }
    }
    if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
      error('Build // Unit tests // Packaging.');
    }
  }

  // sonarQube step to get qualityGate result
  stage('Quality gate') {
    node("C++") {
      timeout(time: 10, unit: 'MINUTES') {
        def qualityGate = waitForQualityGate()
        if (qualityGate.status != 'OK') {
          currentBuild.result = 'FAIL'
        }
      }
      if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
        error('Quality gate failure: ${qualityGate.status}.');
      }
    }
  }

  if ((env.BUILD == 'RELEASE') || (env.BUILD == 'QA')) {
    stage('Delivery') {
      node("C++") {
        unstash 'el7-rpms'
        unstash 'alma8-rpms'
        checkoutCentreonBuild()
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-delivery.sh"
      }
      if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
        error('Delivery stage failure.');
      }
    }

    if (env.BUILD == 'REFERENCE') {
      build job: "centreon-web/${env.BRANCH_NAME}", wait: false
    }
  }
}
finally {
  buildStatus = currentBuild.result ?: 'SUCCESS';
  if ((buildStatus != 'SUCCESS') && ((env.BUILD == 'RELEASE') || (env.BUILD == 'REFERENCE'))) {
    slackSend channel: '#monitoring-metrology', message: "@channel Centreon Broker build ${env.BUILD_NUMBER} of branch ${env.BRANCH_NAME} was broken by ${source.COMMITTER}. Please fix it ASAP."
  }
}
