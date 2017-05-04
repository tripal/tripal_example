
The Tripal API provides a class-based Tripal Fields API for creating custom fields to be used with Tripal Content Types. For a refresher on the terminology surrounding Tripal Content Types and Fields see [Terminology on Tripal.info](http://tripal.info/node/347).

The Tripal Example: Tripal Fields module provides the following examples to aid developers in creating their own custom Tripal Fields.

## Alter existing field display (Comming Soon!)
This example shows you how to add a custom display to an existing Tripal/Chado Field. This is done by creating a custom <em>Tripal Field Formatter</em> and making it available to the field you would like to customize the display for. This method replaces the Tripal2 approach of creating custom template files.

## Add new content to Tripal Content Type(s)
The best way to add new content to a Tripal Content Type(s) is to create a custom field. This example shows you how to add a stock summary to your Tripal Organism pages by creating a custom field to pull out the summary numbers, a default widget to block editing of the summary info and a custom field formatter to display the summary as a table.

Demonstrates:
- Create a custom field, widget and formatter.
- Make a field available to be added by an administrator (makes it optional).
- How to return multiple values while integrating properly with web services.
- Ajax loading of fields to ensure fast page loads for Tripal Content.
