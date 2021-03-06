<?php

function save_dlts_book( $node ) {

  global $user;
  
  $book_exist = book_nid($node['identifier']);
  
  if ($book_exist) {
  
    // Load the node by NID
    $entity = node_load($book_exist);
    
    // Wrap it with Entity API
    $ewrapper = entity_metadata_wrapper('node', $entity);

  }
  
  else {
  
    // entity_create replaces the procedural steps in the first example of
    // creating a new object $node and setting its 'type' and uid property
    $values = array(
      'type' => 'dlts_book',
      'uid' => $user->uid,
      'status' => 1,
      'comment' => 0,
      'promote' => 0,
    );
  
    $entity = entity_create('node', $values);
  
    // The entity is now created, but we have not yet simplified use of it.
    // Now create an entity_metadata_wrapper around the new node entity
    // to make getting and setting values easier
    $ewrapper = entity_metadata_wrapper('node', $entity);
  
  }
  
  $ewrapper->title->set( $node['title'] );
  
  $entity->language = $node['language']; // we only set language to the book object
  
  $pages = (array) $node['pages'];

  $publication_date = $node['publication_date'];

  $scan_date = $node['scan_date'];

  $handle = $node['handle'];
  
  $ewrapper->field_identifier->set( $node['identifier'] );
  
  $collections_nids = array();
  
  foreach($node['collections'] as $collection) {
    $nid = collection_nid($collection);
    if ($nid) {
      $collections_nids[] = $nid;
    }
    else {
      drush_log( t('Book "@title" is part of collection "@collection", but collection does not exist. Skip.', array( '@title' => $node['title'], '@collection' => $collection, ) ), 'warning' );
    }
  }

  $multivol_nids[] = array();
  
  foreach($node['multivol'] as $multivol) {
    drush_log($multivol['identifier'], 'ok');
    $nid = multivol_nid($multivol['identifier']);
    if ($nid) {
      $multivol_nids[] = $nid;
    }
    else {
      drush_log( t('Book "@title" is part of multi volume set "@multivol", but multi volume does not exist. Skip.', array( '@title' => $node['title'], '@multivol' => $multivol['identifier'], ) ), 'warning' );
    }
  }  
  
  $ewrapper->field_collection->set( $collections_nids );
  
  if (!empty($node['isbn'])) {
    $ewrapper->field_isbn->set( $node['isbn'] );
  }

  if (!empty($node['handle'])) {
    $ewrapper->field_handle->set( array( 'url' => $node['handle'] ) );  
  }
  
  if (!empty($node['title_long'])) {
    $ewrapper->field_title->set( $node['title_long'] );  
  }  
  
  if (!empty($node['subtitle'])) {
    $ewrapper->field_subtitle->set( $node['subtitle'] );  
  }  
  
  if (!empty($node['description'])) {
    $ewrapper->field_description->set( $node['description'] );  
  }  
  
  if (!empty($node['page_count'])) {
    $ewrapper->field_page_count->set( $node['page_count'] );  
  }  
  
  if (!empty($node['editor'])) {
    $ewrapper->field_editor->set( $node['editor'] );  
  }

  if (!empty($node['creator'])) {
    $ewrapper->field_creator->set( $node['creator'] );  
  }
  
  if (!empty($node['author'])) {
    $ewrapper->field_author->set( $node['author'] );  
  }  
  
  if (!empty($node['publisher'])) {
    $ewrapper->field_publisher->set( $node['publisher'] );  
  }  
  
  if (!empty($node['contributor'])) {
    $ewrapper->field_contributor->set( $node['contributor'] );  
  }  
  
  if (!empty($node['dimensions'])) {
    $ewrapper->field_dimensions->set( $node['dimensions'] );  
  }

  if (!empty($node['volume'])) {
    $ewrapper->field_volume->set( $node['volume'] );  
  }
  
  if (!empty($node['number'])) {
    $ewrapper->field_number->set( $node['number'] );  
  }    
  
  if (!empty($node['call_number'])) {
    $ewrapper->field_call_number->set( $node['call_number'] );  
  }
  
  if (!empty($node['other_version'])) {
    $ewrapper->field_other_version->set( $node['other_version'] );
  }

  if (!empty($node['binding_orientation'])) {
    $ewrapper->field_binding_orientation->set( $node['binding_orientation'] );
  }

  if (!empty($node['scan_order'])) {    
    $ewrapper->field_scan_order->set( $node['scan_order'] );
  }

  if (!empty($node['read_order'])) {  
    $ewrapper->field_read_order->set( $node['read_order'] );
  }
  
  if (!empty($node['publisher'])) {
    $ewrapper->field_publisher->set( $node['publisher'] );  
  }
  
  if (!empty($node['representative_image'])) {
    $ewrapper->field_representative_image ->set( array('fid' => $node['representative_image']->fid ) ); 
  }
  
  $ewrapper->field_sequence_count->set( count($pages) );
  
  //if (!empty($node['subjects'])) {
  //  $ewrapper->field_subject->set( $node['subjects'] );  
  //}
  
  $ewrapper->save(); 
  
  return $entity;
  
}

function create_dlts_book($books) {
	
  foreach ($books as $value) {
      
    $xml = simplexml_load_file($value->uri);
    
    if (!$xml) drush_log( t('File @file not found', array('@file' => $value->uri)), 'error');

    foreach ($xml->node as $child) {
      
      $pages_array =
        $multivols =
        $other_versions =
        $isbns =
        $authors =
        $collections =
        $contributors =
        $creators =
        $editors =
        $publishers =
        $subjects = array();

      /** book pages */
      foreach ($child->pages->children() as $page) {

        $page = (array) $page;

        $page['service_copy'] = (array) $page['service_copy'];

        $page['cropped_master'] = (array) $page['cropped_master'];

        $pages_array[] = (array) $page;

      }
    
      /** contributors */ 
      foreach ($child->contributors->children() as $contributor) {
        $contributors[] = (string) $contributor[0];
      }

      /** authors */    
      foreach ($child->authors->children() as $author) {
        $authors[] = (string) $author[0];
      }

      /** collections the book belong */
      foreach ($child->isPartOf->collection as $collection) {
        $collections[] = (string) $collection[0];
      }
	  
	  foreach ($child->isPartOf->multivol as $multivol) {
        $multivols[] = array(
          'identifier' => (string) $multivol[0],
          'volume' => (int) $multivol->attributes()->volume[0], 
		);
	  }

      /** creators */
      foreach ($child->creators->children() as $creator) {
        $creators[] = (string) $creator[0];
      }      
      
      /** editor */
      foreach ($child->editors->children() as $editor) {
        $editors[] = (string) $editor[0];
      }     

      /** publishers */
      foreach ($child->publishers->children() as $publisher) {
        $publishers[] = (string) $publisher[0];
      }     
      
      /** subjects */
      foreach ($child->subjects->children() as $subject) {
        $subjects[] = (string) $subject[0];
      }     

      /** isbns */
      foreach ($child->isbns->children() as $isbn) {
        $isbns[] = (string) $isbn[0];
      }     
      
      /** other_versions */
      foreach ($child->other_versions->children() as $other_version) {
        $other_versions[] = array(
          'url' => (string) $other_version[0]
        );
      }           

      $book_identifier = (string)$child->identifier[0];
      
      $book_title = (string)$child->title[0];
      
      $representative_image_source = (string) $child->representative_image->uri;
      
      $representative_image = add_file( '/Users/ortiz/tools/sites/books/sites/all/dummy_book/inc/dummy_book/source/images/' . $representative_image_source);
      
      $node = array(
        'title' => $book_title,
        'title_long' => (string)$child->title_long[0],        
        'status' => (string) $child->status[0],
        'language' => (string) $child->language[0],
        'identifier' => (string) $child->identifier[0],
        'subtitle' => (string) $child->subtitle[0],
        'page_count' => (int) $child->page_count[0],
        'binding_orientation' => (int)$child->binding_orientation[0],
        'call_number' => (string)$child->call_number[0],
        'description' => (string)$child->description[0],
        'dimensions' => (string)$child->dimensions[0],
        'handle' => (string) $child->handle[0],
        'language' => (string)$child->language[0],
        'language_code' => (string)$child->language_code[0],
        'number' => (string)$child->number[0],
        'other_version' => $other_versions,
        'read_order' => (int)$child->read_order[0],
        'representative_image' => $representative_image,
        'pdf_file' => (string)$child->pdf_file[0],
        'scan_order' => (int)$child->scan_order[0],
        'scanning_notes' => (string)$child->scanning_notes[0],
        'volume' => (string)$child->volume[0],
        'publication_date' => (string)$child->publication_date[0],
        'scan_date' => (string)$child->scan_date[0],
        'isbn' => $isbns,
        'identifier' => $book_identifier,
        'collections' => $collections,
        'author' => $authors,
        'creator' => $creators,
        'contributor' => $contributors,
        'sequence_count' => count($pages_array),
        'editor' => $editors,
        'publisher' => $publishers,
        'subjects' => $subjects,
        'pages' => $pages_array,
        'multivol' => $multivols,        
      );
      
      $created = save_dlts_book($node);
      
      if (
        $created && 
        isset($created->nid)
	  ) {

        drush_log(t('Book "@title" was successfully processed. (nid: @nid)', array('@nid' => $created->nid, '@title' => $created->title )), 'ok');

        foreach($pages_array as $key => $book_page) {

          $cropped_master_filename = $book_page['cropped_master']['uri'];

          $service_copy_filename = $book_page['service_copy']['uri'];
          
          $cropped_master = add_file('/Users/ortiz/tools/sites/books/sites/all/dummy_book/inc/dummy_book/source/images/' . $cropped_master_filename);

          $service_copy = add_file('/Users/ortiz/tools/sites/books/sites/all/dummy_book/inc/dummy_book/source/images/' . $service_copy_filename);

          $title = (isset($book_page['title'])) ? $book_page['title'] : $book_title . ' ' . t('page @number', array('@number' => $book_page['sequence_number']));

          $page_node = array(
            'book' => $created->nid,
            'status' => 1,            
            'language' => LANGUAGE_NONE, // only the book object can have multiple language
            'is_part_of' => $book_identifier,
            'title' => $title,
            'hand_side' => $book_page['hand_side'],
            'page_type' => $book_page['page_type'],
            'real_page_number' => $book_page['real_page_number'],
            'sequence_number' => $book_page['sequence_number'],
            'visible' => $page['visible'],
            'cropped_master' => $cropped_master,  
            'service_copy' => $service_copy,          
          );

          $this_book_page = create_dlts_book_page( $page_node );

          if ($this_book_page->nid) {
             drush_log( t('Book page "@title" part of "@book" was successfully processed. (nid: @nid)', array('@nid' => $this_book_page->nid, '@title' => $this_book_page->title, '@book' => $created->title )), 'ok');
          }

        }
      }
    }
  }
}