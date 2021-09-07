@Library('centreon-shared-library') _

/*
** Variables.
*/
properties([buildDiscarder(logRotator(numToKeepStr: '10'))])
def serie = '21.10'
def maintenanceBranch = "${serie}.x"
def qaBranch = "dev-${serie}"

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

stage('Build // Unit tests // Packaging') {
  node("C++") {
    sh 'setup_centreon_build.sh'
    sh "./centreon-build/jobs/broker/${serie}/mon-broker-package.sh centos7"
    stash name: 'el7-rpms', includes: "output/x86_64/*.rpm"
    archiveArtifacts artifacts: "output/x86_64/*.rpm"
    sh 'rm -rf output'
  }
}

stage('Delivery') {
  node("C++") {
    unstash 'el7-rpms'
    sh "MAJOR=`echo $VERSION | cut -d . -f 1,2`"
    sh "EL7RPMS=`echo output/x86_64/*.el7.*.rpm`"
    putRpms "standard" "$MAJOR" "el7" "unstable" "x86_64" "broker" "centreon-broker-$VERSION-$RELEASE" $EL7RPMS
  }  
}
