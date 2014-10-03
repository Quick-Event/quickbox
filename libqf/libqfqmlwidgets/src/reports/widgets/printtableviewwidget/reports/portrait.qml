import qf.qmlreports 1.0
import qf.qmlreports.Style 1.0

Report {
	id: root
	styleSheet: Sheet {
		name: "ahoj"
	}
	// 	basedOn: MyStyle {}
	// 	colors: [
	// 		Color {name: "red"; definition: "#FF0000"}
	// 	]
	// }
	width: 210
	height: 297
	vinset: 10
	hinset: 5
}

// <?xml version='1.0' encoding='utf-8'?>
// <report w="210" headeronbreak="1" h="297" orientation="portrait" vinset="5" hinset="5">
//  <stylesheet>
// 	<colors>
// 		<color name="black" definition="#000"/>
// 		<color name="gray20" definition="#CDCDCD"/>
// 		<color name="gray10" definition="#DCDCDC"/>
// 		<color name="white" definition="#FFFFFF"/>
// 		<color name="header" definition="#444444"/>
// 		<color name="honey" definition="RGB(255,244,148)"/>
// 		<color name="peach" definition="RGB(255,223,83)"/>
// 		<color name="waterprint" definition="#EEEEEE"/>
// 		<color name="errorfill" definition="lightcoral"/>
// 	</colors>
// 	<pens>
// 		<pen name="black1" size="1"/>
// 		<pen name="black2" basedon="black1" size="2"/>
// 		<pen name="blue1" basedon="black1" color="blue"/>
// 		<pen name="white1" basedon="black1" color="white"/>
// 		<pen name="blue2" basedon="blue1" size="2"/>
// 		<pen name="blackdot05" basedon="black1" size="0.5" style="1,4" cap="round"/>
// 		<pen name="blackdot1" basedon="black1" style="dot" cap="round"/>
// 		<pen name="blackdash1" basedon="black1" style="dash" cap="flat"/>
// 		<pen name="bluedash1" basedon="blackdash1" color="blue"/>
// 		<pen name="blackdash05" basedon="blackdash1" size="0.5"/>
// 		<pen name="header" basedon="black1" size="0.5" color="header"/>
// 		<pen name="graphaxis" basedon="black1" size="0.5"/>
// 		<pen name="graphaxes" basedon="graphaxis"/>
// 		<pen name="graphgrid" basedon="graphaxis" style="dash"/>
// 	</pens>
// 	<brushes>
// 		<brush name="error" color="errorfill"/>
// 		<brush name="white" color="white"/>
// 		<brush name="gray20" color="gray20"/>
// 		<brush name="peach" color="#FFF4BD"/>
// 		<brush name="waterprint" color="waterprint"/>
// 		<brush name="shadow" color="gray10"/>
// 		<brush name="tblshadow" basedon="shadow"/>
// 	</brushes>
// 	<fonts>
// <!--		<font name="sans" family="Nimbus Sans L" size="9"/>-->
// 		<font name="sans" family="Arial" size="9"/>
// 		<font name="sansB" basedon="sans" weight="bold"/>
// 		<font name="sansL" basedon="sans" size="+2"/>
// 		<font name="sansLB" basedon="sansL" weight="bold"/>
// 		<font name="sansXL" basedon="sansL" size="+2"/>
// 		<font name="sansXXL" basedon="sansXL" size="+4"/>
// 		<font name="sansS" basedon="sans" size="-1"/>
// 		<font name="sansSI" basedon="sansS" style="italic"/>
// 		<font name="sansSB" basedon="sansS" weight="bold"/>
// 		<font name="monospace" family="bitstream vera sans mono" size="9"/>
// 		<font name="serif"  basedon="sans" family="serif"/>
// 		<font name="tbltext" basedon="sans"/>
// 		<font name="tbltextB" basedon="tbltext" weight="bold"/>
// 		<font name="tblhead" basedon="tbltextB"/>
// 		<font name="tblbig" basedon="sansL" size="+9"/>
// 		<font name="tblbigB" basedon="tblbig" weight="bold"/>
// 		<font name="header" basedon="sans" size="-1"/>
// 		<font name="note" basedon="sansS"/>
// 		<font name="H1" basedon="sansXXL" weight="bold"/>
// 		<font name="H2" basedon="sansXL" weight="bold"/>
// 		<font name="H3" basedon="sansL" weight="bold"/>
// 	</fonts>
// 	<styles>
// 		<style name="default" font="sans"/>
// 		<style name="header" font="header" pen="header"/>
// 		<style name="H1" font="H1" pen="color: black"/>
// 		<style name="H2" font="H2" pen="color: maroon"/>
// 		<style name="H3" font="H3" pen="color: black"/>
// 		<style name="reportheading" basedon="H1"/>
// 		<style name="graphheading" basedon="H2"/>
// 		<style name="tbltext" font="tbltext"/>
// 		<style name="tbltextB" font="tbltextB"/>
// 		<style name="tblhead" font="tblhead" pen="color: maroon"/>
// 		<style name="tblheading" basedon="tblhead"/>
// 		<style name="tbllabel" font="tbltext; size:-1" pen="color: maroon"/>
// 		<style name="tblcolname" font="tbltext" pen="color: maroon"/>
// 		<style name="label" font="tbltext" pen="color: maroon"/>
// 		<style name="note" font="note" pen="color:darkslategray"/>
// 	</styles>
//  </stylesheet>
//  <body w="%" h="%" id="body">
//   <band datatablename="report">
// 	<para w="%" style="reportheading" halign="center" >
// 		<data domain="table" src="title" />
// 	</para>
// 	<space h="5" />
// 	<para w="%">
// 		<data domain="table" src="note" />
// 	</para>
// 	<space h="5" />
//   </band>
//   <table w="%" datatablename="data" headeronbreak="true" createfromdata="true" decoration="grid">
// 	<detailframe  keepall="0"/>
//   </table>
//  </body>
//  <row valign="top" w="%" bbrd="header" keepall="1" layout="horizontal">
//    <row w="%" valign="top">
// 		<para style="header" w="%" halign="left">
// 			<data domain="system" src="date"/>\s<data domain="system" src="time"/>
// 		</para>
// 		<para style="header" w="%" halign="right">strana <data domain="system" src="page"/>/<data domain="system" src="pageCount"/></para>
//    </row>
//  </row>
// </report>
