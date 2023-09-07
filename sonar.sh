# static check
mkdir -p .reports
mkdir -p .logs

# https://stackoverflow.com/questions/6986033/cppcheck-cant-find-include-files
cppcheck -itest include src \
--include=/home/xavier/mambaforge/envs/smartlabel/include/zip.h \
--suppress=missingIncludeSystem . \
--std=c++20 \
-v --enable=all -I include src --xml  2>.reports/cppcheck.xml

# sonar 9.9.1
/home/xavier/Codes/sonar-scanner-5.0.1.3006-linux/bin/sonar-scanner -X \
  -Dsonar.projectKey=zip_helper \
  -Dsonar.sources=include,src \
  -Dsonar.host.url=http://localhost:9000 \
  -Dsonar.login=sqp_c930f03b46342c748ab0df39ece313136960ebdd \
  &>.logs/sonar-scanner.log

# # clean up
# rm -rf .coverage
# rm -rf .pytest_cache
# rm -rf .reports
# rm -rf .logs
# rm -rf .scannerwork
