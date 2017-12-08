<?php
/**
 * @file
 * Stanford CAPx administration pages.
 */

namespace Drupal\stanford_capx\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\node\Entity\Node;
use Drupal\file\Entity\File;
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
    $config = \Drupal::config('stanford_capx.settings');
    $username = $config->get("username");
    $password = $config->get("password");
    $profileID = "53006";
    $sunetID = "sheamck";
    $guzzle = new Client(['defaults' => ['auth' => 'oauth']]);

    $auth = new Auth($guzzle);
    $auth->authenticate($username, $password);
    $client = new CAPAPI($guzzle, $auth);

    $profile = $client->api('profile')->get($profileID);

    // var_dump($profile);

    $links = [];
    foreach ($profile->internetLinks as $key => $v) {
      dump($v);
      $links[] = [
        'title' => $v->label->text,
        'url' => $v->url
      ];
    }

    // Create node object with attached file.
    $node = Node::create([
      'type' => 'stanford_person',
      'title' => $profile->displayName,
      'field_s_person_bio' => $profile->bio->html,
      'field_s_person_email' => $profile->uid . "@stanford.edu",
      'field_s_person_first_name' => $profile->names->legal->firstName,
      'field_s_person_last_name' => $profile->names->legal->lastName,
      'field_s_person_links' => $links,
    ]);
    $node->save();

    return [
      '#markup' => $content,
    ];
  }

}
