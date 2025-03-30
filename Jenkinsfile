pipeline{
    agent any
    stages{
        stage('Checkout'){
            steps{
                echo 'Checkout complete!'
            }
        }
        stage('Backtest Model'){
            steps{
                echo 'Starting backtest...'
                dir('strategy'){
                    sh 'ls'
                    sh 'make'
                    sh 'ls'
                    sh 'sh go.sh'
                }
                echo 'Backtest complete!'
            }
        }
        stage('Push Results'){
            steps{
                echo 'Starting push...'
                sh 'cp -r /home/vagrant/ss/bt/backtesting-results/ .; git add backtesting-results; git commit -m "Backtesting Results"; git push'
                echo 'Pushing output complete!'
            }
        }
        stage('Cleaning'){
            steps{
                echo 'Starting clean...'
                deleteDir()
                echo 'Cleaning complete!'
            }
        }
    }
}