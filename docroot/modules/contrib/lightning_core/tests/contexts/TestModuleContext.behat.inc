<?php

namespace Acquia\LightningExtension\Context;

use Behat\Behat\Hook\Scope\AfterScenarioScope;
use Behat\Behat\Hook\Scope\BeforeScenarioScope;
use Behat\Behat\Hook\Scope\ScenarioScope;
use Drupal\Core\Site\Settings;
use Drupal\DrupalExtension\Context\DrupalSubContextBase;

/**
 * Allows test modules to be installed during a scenario and uninstalled after.
 */
final class TestModuleContext extends DrupalSubContextBase {

  /**
   * Determines the test modules to install during the scenario.
   *
   * @param ScenarioScope $scope
   *   The current scenario scope.
   *
   * @return string[]
   *   The test modules to install.
   */
  private function getModules(ScenarioScope $scope) {
    $modules = [];

    $tags = array_merge(
      $scope->getFeature()->getTags(),
      $scope->getScenario()->getTags()
    );
    foreach ($tags as $tag) {
      if (strpos($tag, 'with-module:') === 0) {
        $modules[] = substr($tag, 12);
      }
    }
    return array_unique($modules);
  }

  /**
   * Installs test modules for this scenario.
   *
   * @param BeforeScenarioScope $scope
   *   The scenario scope.
   *
   * @BeforeScenario
   */
  public function install(BeforeScenarioScope $scope) {
    $modules = $this->getModules($scope);

    if ($modules) {
      // Bootstrap Drupal if needed.
      $driver = $this->getDriver();
      if (! $driver->isBootstrapped()) {
        $driver->bootstrap();
      }

      // Ensure that test directories are scanned for modules.
      $settings = Settings::getAll();
      $settings['extension_discovery_scan_tests'] = TRUE;
      new Settings($settings);

      \Drupal::service('module_installer')->install($modules);
    }
  }

  /**
   * Uninstalls any test modules installed for the scenario.
   *
   * @param AfterScenarioScope $scope
   *   The scenario scope.
   *
   * @AfterScenario
   */
  public function uninstall(AfterScenarioScope $scope) {
    $modules = $this->getModules($scope);

    if ($modules) {
      \Drupal::service('module_installer')->uninstall($modules);
    }
  }

}
