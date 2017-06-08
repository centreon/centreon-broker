stage('Source') {
  node {
    sh 'cd /opt/centreon-build && git pull && cd -'
    dir('centreon-broker') {
      checkout scm
    }
    sh '/opt/centreon-build/jobs/broker/3.4/mon-broker-source.sh'
    source = readProperties file: 'source.properties'
    env.VERSION = "${source.VERSION}"
    env.RELEASE = "${source.RELEASE}"
  }
}

stage('Unit tests') {
  parallel 'centos6': {
    node {
      sh 'cd /opt/centreon-build && git pull && cd -'
      sh '/opt/centreon-build/jobs/broker/3.4/mon-broker-unittest.sh centos6'
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
  'centos7': {
    node {
      sh 'cd /opt/centreon-build && git pull && cd -'
      sh '/opt/centreon-build/jobs/broker/3.4/mon-broker-unittest.sh centos7'
      step([
        $class: 'XUnitBuilder',
        thresholds: [
          [$class: 'FailedThreshold', failureThreshold: '0'],
          [$class: 'SkippedThreshold', failureThreshold: '0']
        ],
        tools: [[$class: 'GoogleTestType', pattern: 'ut.xml']]
      ])
    }
  }
  if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
    error('Unit tests stage failure.');
  }
}

stage('Package') {
  parallel 'centos6': {
    node {
      sh 'cd /opt/centreon-build && git pull && cd -'
      sh '/opt/centreon-build/jobs/broker/3.4/mon-broker-package.sh centos6'
    }
  },
  'centos7': {
    node {
      sh 'cd /opt/centreon-build && git pull && cd -'
      sh '/opt/centreon-build/jobs/broker/3.4/mon-broker-package.sh centos7'
    }
  }
  if ((currentBuild.result ?: 'SUCCESS') != 'SUCCESS') {
    error('Package stage failure.');
  }
}

if (env.BRANCH_NAME == 'master') {
  build job: 'centreon-web/master', wait: false
}
