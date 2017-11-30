<?php
/**
 * @file
 * Stanford CAPx administration pages.
 */

namespace Drupal\stanford_capx\Controller;

use Drupal\Core\Controller\ControllerBase;
use SUSWS\CAPAPI\CAPAPI;
use SUSWS\APIAuthLib\Auth;
use GuzzleHttp\Client;

/**
 * Controller routines for page example routes.
 */
class TestPageController extends ControllerBase {

  /**
   * {@inheritdoc}
   */
  protected function getModuleName() {
    return 'stanford_capx';
  }

  /**
   * Render out test page data.
   * @return [type] [description]
   */
  public function test() {
    $content = "";
    $username = "example";
    $password = "example";
    $profileID = "53006";
    $sunetID = "sheamck";
    $guzzle = new Client(['defaults' => ['auth' => 'oauth']]);

    $auth = new Auth($guzzle);
    $auth->authenticate($username, $password);
    $client = new CAPAPI($guzzle, $auth);

    $profile = $client->api('profile')->get($profileID);

    var_dump($profile);

    return [
      '#markup' => $content,
    ];
  }

}
