<?php
/**
 * @file
 * Stanford CAPx administration pages.
 */

namespace Drupal\stanford_capx\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * Controller routines for page example routes.
 */
class AdminPagesController extends ControllerBase {

  /**
   * {@inheritdoc}
   */
  protected function getModuleName() {
    return 'stanford_capx';
  }

  /**
   * Athentication Credentials Input Page.
   *
   * @return [type] [description]
   */
  public function authenticationPage() {
    $content = [];
    $content['authform'] = \Drupal::formBuilder()
      ->getForm('Drupal\stanford_capx\Form\AuthenticationForm');

    return $content;
  }

  /**
   * Settings Page.
   *
   * @return [type] [description]
   */
  public function settingsPage() {
    $content['#markup'] = "<p>These are the default configuration settings for importing data from the CAP Network at https://cap.stanford.edu/</p>";
    $content['settingsForm'] = \Drupal::formBuilder()
                                  ->getForm('Drupal\stanford_capx\Form\SettingsForm');

    return $content;
  }

  /**
   * Mapping Page.
   *
   * @return [type] [description]
   */
  public function mappingPage() {
    $link_options = array('attributes' => array('class' => array('btn button')), 'query' => array('destination' => Url::fromRoute('<current>')->toString()));
    $create_url = URL::fromUri("internal:/admin/config/capx/mapping/create");
    $create_url->setOptions($link_options);
    $create_link = Link::fromTextAndUrl(t("Create new Mapping"), $create_url)->toString();

    $import_url = URL::fromUri("internal:/admin/config/capx/mapping/import");
    $import_url->setOptions($link_options);
    $import_link = Link::fromTextAndUrl(t("Import Mapping"), $import_url)->toString();

    $content['#markup']  = "<p>After you have connected to CAP, create a Mapping to link CAP’s fields with your fields.</p>";
    $content['#markup']  = "<p>" . $create_link . " " . $import_link . "</p>";
    $content['mappingForm'] = \Drupal::formBuilder()
                                      ->getForm('Drupal\stanford_capx\Form\MappingForm');
    return $content;
  }

  /**
   * Create Mapping Page.
   *
   * @return [type] [description]
   */
  public function createMappingPage() {
    $content['#markup']  = "<p>Create a new mapping to link CAP’s fields with your fields.</p>";
    $content['createMappingForm'] = \Drupal::formBuilder()
                                      ->getForm('Drupal\stanford_capx\Form\CreateMappingForm');
    return $content;
  }

 /**
   * Import Mapping Page.
   *
   * @return [type] [description]
   */
  public function importMappingPage() {
    $content['#markup']  = "<p>Import mapping configuration.</p>";
    $content['importMappingForm'] = \Drupal::formBuilder()
                                      ->getForm('Drupal\stanford_capx\Form\importMappingForm');
    return $content;
  }

  /**
   * Import Page.
   *
   * @return [type] [description]
   */
  public function importPage() {
    $link_options = array('attributes' => array('class' => array('btn button')), 'query' => array('destination' => Url::fromRoute('<current>')->toString()));
    $url = URL::fromUri("internal:/admin/config/capx/importer/create");
    $url->setOptions($link_options);
    $link = Link::fromTextAndUrl(t("Create new Importer"), $url)->toString();

    $content['#markup'] = "<p>Importers allow you to chose CAP profiles in bulk by Organizations, or Workgroups, or SunetIDs</p>";
    $content['#markup'] .= "<p>" . $link . "</p>";

    return $content;
  }

  /**
   * Create Importer Page.
   *
   * @return [type] [description]
   */
  public function createImporterPage() {
    $content['#markup'] = "<p>After you have a mapping configured, create an importer to chose which profiles you would like to import.</p>";
    $content['createImporterForm'] = \Drupal::formBuilder()
                                           ->getForm('Drupal\stanford_capx\Form\createImporterForm');

    return $content;
  }

  /**
   * Profiles Page.
   *
   * @return [type] [description]
   */
  public function profilesPage() {
    $content = "<p>These are the profiles currently imported into your site.</p>";
    return [
      '#markup' => $content,
    ];
  }

  /**
   * Help Page.
   *
   * @return [type] [description]
   */
  public function helpPage() {

    $content = "<h2>" . t("Getting started") . "</h2>";
    $content .= "<p>" . t("Importing content from CAP can be completed in 3 steps:") . "</p>";
    $content .= "<ol><li>";
    $url = URL::fromRoute("stanford_capx.authentication_credentials");
    $link = link::fromtextandurl(t("Settings"), $url)->toString();
    $content .= t("@connect - Connect to the CAP API using your authentication credentials.", array("@connect" => $link));
    //$content .= t("!connect - Connect to the CAP API using your authentication credentials.", array("!connect" => l(t("Settings"), "admin/config/capx/authentication")));
    $content .= "</li><li>";
    //$content .= t("!mapping - Create a mapping that links CAP fields to your own fields.", array("!mapping" => l(t("Mapping"), "admin/config/capx/map")));
    $content .= "</li><li>";
    //$content .= t("!import - Choose which profiles you would like to import.", array("!import" => l(t("Import"), "admin/config/capx/importer")));
    $content .= "</li></ol>";
    $content .= "<p>&nbsp;</p>";

    $content .= "<h2>" . t("Authentication credentials") . "</h2>";
    $content .= "<p>" . t("Authentication credentials allow you to connect to the CAP API and import content into your website.") . "</p>";
    //$content .= "<p>" . t("To get authentication credentials, file a !helpsu to Administrative Applications/CAP Stanford Profiles.", array("!helpsu" => l(t("HelpSU request"), "https://helpsu.stanford.edu/helpsu/3.0/auth/helpsu-form?pcat=CAP_API&dtemplate=CAP-OAuth-Info"))) . "</p>";
    $content .= "<p>&nbsp;</p>";

    $content .= "<h2>" . t("CAP data - Choosing fields") . "</h2>";
    //$content .= "<p>" . t("If you need help determining which fields you need in your mapping, you can use the !caplink. The schema displays all fields in CAP and where they are nested. Yes, it is a ton of data, we know...", array("!caplink" => l(t("CAP Data Schema"), "admin/config/capx/data-browser"))) . "</p>";
    $content .= "<p><strong>" . t("Common fields: ") . "</strong>";
    //$content .= t("A list of common fields can be found on the sidebar of any !mappingpage.", array("!mappingpage" => l(t("mapping page"), "admin/config/capx/map/new"))) . "</p>";
    $content .= "<p>&nbsp;</p>";

    $content .= "<h2>" . t("Resources for developers") . "</h2>";
    $content .= "<p>" . t("Want to get involved in the development of the CAPx module? Check out the resources below:") . "</p>";
    $content .= "<ol><li>";
    /*
    $content .= l(t("CAPx module documentation"), "https://github.com/SU-SWS/stanford_capx/tree/8.x-1.x/docs");
    $content .= "</li><li>";
    $content .= l(t("CAPx GitHub repository"), "http://www.github.com/SU-SWS/stanford_capx");
    $content .= "</li><li>";
    $content .= l(t("CAPx issue queue"), "https://github.com/SU-SWS/stanford_capx/issues");
    $content .= "</li><li>";
    $content .= l(t("Working group Jira project"), "https://stanfordits.atlassian.net/");
    */
    $content .= "</li></ol>";

    return [
      '#markup' => $content,
    ];
  }

}
