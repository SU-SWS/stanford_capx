# Drupal CAP Library

This is a PHP library that handles data from the CAP API and turns it into useful information for Drupal. The concept of this library is to be able to map information from CAP Profile data directly into an existing entity type through the UI. Included in this library is a collection of PHP classes that work together to allow for multiple configurations and options.

## Documentation

* Importers are objects that define who/what gets imported and contain mappers. [Read More](Importer/README.md)
* Mappers are objects that define what CAP data goes into which entity's fields. Mappers contain processors to assist in this transaction. [Read More](Mapper/README.md)
* [Processors](Processors/README.md) are objects that turn CAP data into a format that Drupal can use. There are two types of Processors; [Property](PropertyProcessors/README.md) and [Field](FieldProcessors/README.md).
* Utility functions are common functions that are globally available. [Read More](Util/README.md)


## Requirements

* CAPx APILib PHP library for communicating with the CAP API.
* Guzzle PHP Library
* Drupal Entity Module
* Drupal Date Module
* PHP >= 5.2.3 probably with cURL

## Usage

      $username = decrypt(variable_get('stanford_capx_username', ''));
      $password = decrypt(variable_get('stanford_capx_password', ''));
      $token    = variable_get('stanford_capx_token', '');

      $connection = CAPx::testConnectionToken($token);

      if (!$connection->value) {
        $client = new HTTPClient();
        $response = $client->api('auth')->authenticate($username, $password);
        if ($response) {
          $token = $response->getAuthApiToken();
          variable_set('stanford_capx_token', $token);
        }
        else {
          throw new \Exception("Could not authenticate with API server.");
        }
      }

      $importer_config = array();
      $importer_config['type']    = "uids";
      $importer_config['values']  = array('nameone','nametwo','namethree','etc');

      $mapper_config = array();

      $mapper_config['entityType']        = 'node';
      $mapper_config['bundleType']        = 'faux'; // name of content type
      $mapper_config['fields']            = array(); // to hold fields
      $mapper_config['properties']        = array(); // to hold properties
      $mapper_config['fieldCollections']  = array(); // to hold field collection fields.

      // FIELDS
      // ---------------------------------------------------------------------------

      // This setting is exactly the same as the commented out line below.
      $mapper_config['fields']['body']                          = '$.bio.html';
      // $mapper_config['fields']['body']                          = array('value' => '$.bio.html');

      $mapper_config['fields']['field_faux_bool_one']           = "$.affiliations.capFaculty";
      $mapper_config['fields']['field_faux_bool_two']           = "$.affiliations.capStaff";

      $mapper_config['fields']['field_faux_date_date']          = "$.lastModified";
      $mapper_config['fields']['field_faux_date_date_two']      = "$.lastModified";
      $mapper_config['fields']['field_faux_date_iso']           = "$.lastModified";
      $mapper_config['fields']['field_faux_date_iso_two']       = "$.lastModified";
      $mapper_config['fields']['field_faux_date_unix']          = "$.lastModified";
      $mapper_config['fields']['field_faux_date_pop']           = "$.lastModified";

      $mapper_config['fields']['field_faux_decimal']            = "$.presentations.location.latitude";
      $mapper_config['fields']['field_faux_email']              = "$.primaryContact.email";
      $mapper_config['fields']['field_faux_file']               = "$.profilePhotos.big";
      $mapper_config['fields']['field_faux_float']              = "$.presentations.location.latitude";
      $mapper_config['fields']['field_faux_image']              = "$.profilePhotos.bigger";
      $mapper_config['fields']['field_faux_integer']            = "$.professionalOrganizations.startYear.text";

      // Example of how to handle a field with multiple values
      $mapper_config['fields']['field_faux_link']               = array("title" => "$.shortTitle.label.text",
                                                                    "url"   => "$.profilePhotos.big.url");
      $mapper_config['fields']['field_faux_list_float']         = "$.universityId";
      $mapper_config['fields']['field_faux_list_int']           = "$.profileId";
      $mapper_config['fields']['field_faux_list_text']          = "$.shortTitle.label.text";
      $mapper_config['fields']['field_faux_long_text']          = "$.currentRoleAtStanford";
      $mapper_config['fields']['field_faux_long_text_summary']  = array('value' => '$.currentRoleAtStanford',
                                                                    'summary' => '$.bio.html');
      // Terms
      $mapper_config['fields']['field_faux_tax_term']           = "$.affiliations";
      $mapper_config['fields']['field_faux_tax_term_select']    = "$.professionalInterests.text";
      $mapper_config['fields']['field_faux_tax_term_auto']      = "$.professionalInterests.text";

      $mapper_config['fields']['field_faux_text']               = "$.shortTitle.label.text";

      // FIELD COLLECTIONS
      // ---------------------------------------------------------------------------
      $collection_config = array();
      $collection_config['bundleType'] = 'field_faux_collection';
      $collection_config['fields'] = array();
      $collection_config['properties'] = array();
      $collection_config['fields']['field_faux_collection_text']  = "$.shortTitle.label.text";
      $collection_config['fields']['field_faux_collection_terms'] = "$.affiliations";
      $collection_config['fields']['field_faux_image']            = "$.profilePhotos.bigger";


      $collection_mapper = new FieldCollectionMapper($collection_config);
      $mapper_config['fieldCollections']['field_faux_collection']           = $collection_mapper;

      // PROPERTIES
      // ---------------------------------------------------------------------------
      $mapper_config['properties']['title'] = '$.displayName';



      $mapper = new EntityMapper($mapper_config);

      $client = new HTTPClient();
      $client->setApiToken($token);

      $importer = new EntityImporter($importer_config, $mapper, $client);
      $importer->execute();










