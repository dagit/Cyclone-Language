<?xml version="1.0"?>

<!-- A stylesheet for rendering Cyclone values (expressed as XML) into HTML. 

  How to use:  Put the output of the Typerep.xmlize_type into a file 
  named "foo.xml" (or whatever).  

  Then prefix that file with the following lines:

  <?xml version="1.0"?> 
  <?xml-stylesheet type="text/xsl" href="cyc.xsl"?>

  Finally, load foo.xml using your favorite XSLT-compliant browser.
  Recent Mozilla's work, and IE5.5+ "shou;d" work (I haven't checked).
  Also, the Opera browser from W3C should work.

  Otherwise, there are external XSLT engines such as Saxon and Xalan that 
  can do the translation externally and you can then view the resulting HTML
  file.

  -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
                 xmlns="http://www.w3.org/1999/xhtml">

 <xsl:template match="/"> 
  <html> 
   <head>
    <title>Cyclone data</title>
   </head>
   <body><xsl:apply-templates/> 
   </body>
  </html>
 </xsl:template>

 <xsl:template match="tunion">&amp;[(<xsl:apply-templates/>)]</xsl:template>
 <xsl:template match="tuple">$(<xsl:apply-templates/>)</xsl:template>
 <xsl:template match="thinptr">{<xsl:apply-templates/>}</xsl:template>
 <xsl:template match="fatptr">[<xsl:apply-templates/>]</xsl:template>

</xsl:stylesheet>
