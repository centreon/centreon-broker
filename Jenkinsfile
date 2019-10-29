/*
** Variables.
*/
properties([buildDiscarder(logRotator(numToKeepStr: '50'))])
def serie = '19.10'
def maintenanceBranch = "${serie}.x"
if (env.BRANCH_NAME.startsWith('release-')) {
  env.BUILD = 'RELEASE'
} else if ((env.BRANCH_NAME == 'master') || (env.BRANCH_NAME == maintenanceBranch)) {
  env.BUILD = 'REFERENCE'
} else {
  env.BUILD = 'CI'
}

/*
** Pipeline code.
*/
stage('Source') {
  node {
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
  stage('Unit tests') {
    parallel 'centos7': {
      node {
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
        if ((env.BUILD == 'RELEASE') || (env.BUILD == 'REFERENCE')) {
          withSonarQubeEnv('SonarQube') {
            sh "./centreon-build/jobs/broker/${serie}/mon-broker-analysis.sh"
          }
        }
      }
    }
    if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
      error('Unit tests stage failure.');
    }
  }

  stage('Package') {
    parallel 'centos7': {
      node {
        sh 'setup_centreon_build.sh'
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-package.sh centos7"
      }
    }
    if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
      error('Package stage failure.');
    }
  }

  if ((env.BUILD == 'RELEASE') || (env.BUILD == 'REFERENCE')) {
    stage('Delivery') {
      node {
        sh 'setup_centreon_build.sh'
        sh "./centreon-build/jobs/broker/${serie}/mon-broker-delivery.sh"
      }
      if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
        error('Delivery stage failure.');
      }
    }

    if (env.BUILD == 'REFERENCE') {
      build job: 'centreon-web/19.10.x', wait: false
    }
  }
}
finally {
  buildStatus = currentBuild.result ?: 'SUCCESS';
  if ((buildStatus != 'SUCCESS') && ((env.BUILD == 'RELEASE') || (env.BUILD == 'REFERENCE'))) {
    slackSend channel: '#monitoring-metrology', message: "@channel Centreon Broker build ${env.BUILD_NUMBER} of branch ${env.BRANCH_NAME} was broken by ${source.COMMITTER}. Please fix it ASAP."
  }
}
