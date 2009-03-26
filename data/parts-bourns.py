#!/usr/bin/python
##############################################################################
##
##  Create sample parts from Bourns for the GParts database.
##
##############################################################################

print "CALL AddCompany("
print "    'Bourns'"
print "    );"
print

##############################################################################
##
##  CR0603/0805/1206 Chip Resistors
##
##############################################################################

packages = [
    ( '0603', '0603(1608)' ),
    ( '0805', '0805(2012)' ),
    ( '1206', '1206(3216)' )
    ]

resistances = [
    ( '10R0',      10.0 ),
    ( '10R2',      10.2 ),
    ( '10R5',      10.5 ),
    ( '10R7',      10.7 ),
    ( '11R0',      11.0 ),
    ( '11R3',      11.3 ),
    ( '11R5',      11.5 ),
    ( '11R8',      11.8 ),
    ( '12R0',      12.0 ),
    ( '12R1',      12.1 ),
    ( '12R4',      12.4 ),
    ( '12R7',      12.7 ),
    ( '13R0',      13.0 ),
    ( '13R3',      13.3 ),
    ( '13R7',      13.7 ),
    ( '14R0',      14.0 ),
    ( '14R3',      14.3 ),
    ( '14R7',      14.7 ),
    ( '15R0',      15.0 ),
    ( '15R4',      15.4 ),
    ( '15R8',      15.8 ),
    ( '16R0',      16.0 ),
    ( '16R2',      16.2 ),
    ( '16R5',      16.5 ),
    ( '16R9',      16.9 ),
    ( '17R4',      17.4 ),
    ( '17R8',      17.8 ),
    ( '18R0',      18.0 ),
    ( '18R2',      18.2 ),
    ( '18R7',      18.7 ),
    ( '19R1',      19.1 ),
    ( '19R6',      19.6 ),
    ( '20R0',      20.0 ),
    ( '20R5',      20.5 ),
    ( '21R0',      21.0 ),
    ( '21R5',      21.5 ),
    ( '22R0',      22.0 ),
    ( '22R1',      22.1 ),
    ( '22R6',      22.6 ),
    ( '23R2',      23.2 ),
    ( '23R7',      23.7 ),
    ( '24R0',      24.0 ),
    ( '24R3',      24.3 ),
    ( '24R9',      24.9 ),
    ( '25R5',      25.5 ),
    ( '26R1',      26.1 ),
    ( '26R7',      26.7 ),
    ( '27R0',      27.0 ),
    ( '27R4',      27.4 ),
    ( '28R0',      28.0 ),
    ( '28R7',      28.7 ),
    ( '29R4',      29.4 ),
    ( '30R0',      30.0 ),
    ( '30R1',      30.1 ),
    ( '30R9',      30.9 ),
    ( '31R6',      31.6 ),
    ( '32R4',      32.4 ),
    ( '33R0',      33.0 ),
    ( '33R2',      33.2 ),
    ( '34R0',      34.0 ),
    ( '34R8',      34.8 ),
    ( '35R7',      35.7 ),
    ( '36R0',      36.0 ),
    ( '36R5',      35.6 ),
    ( '37R4',      37.4 ),
    ( '38R3',      38.3 ),
    ( '39R0',      39.0 ),
    ( '39R2',      39.2 ),
    ( '40R2',      40.2 ),
    ( '41R2',      41.2 ),
    ( '42R2',      42.2 ),
    ( '43R0',      43.0 ),
    ( '43R2',      43.2 ),
    ( '44R2',      44.2 ),
    ( '45R3',      45.3 ),
    ( '46R4',      46.4 ),
    ( '47R0',      47.0 ),
    ( '47R5',      47.5 ),
    ( '48R7',      48.7 ),
    ( '49R9',      49.9 ),
    ( '51R0',      51.0 ),
    ( '51R1',      51.1 ),
    ( '52R3',      52.3 ),
    ( '53R6',      53.6 ),
    ( '54R9',      54.9 ),
    ( '56R0',      56.0 ),
    ( '56R2',      56.2 ),
    ( '57R6',      57.6 ),
    ( '59R0',      59.0 ),
    ( '60R4',      60.4 ),
    ( '61R9',      61.9 ),
    ( '62R0',      62.0 ),
    ( '63R4',      63.4 ),
    ( '64R9',      64.9 ),
    ( '66R5',      66.5 ),
    ( '68R0',      68.0 ),
    ( '68R1',      68.1 ),
    ( '69R8',      69.8 ),
    ( '71R5',      71.5 ),
    ( '73R2',      73.2 ),
    ( '75R0',      75.0 ),
    ( '76R8',      76.8 ),
    ( '78R7',      78.7 ),
    ( '80R6',      80.6 ),
    ( '82R0',      82.0 ),
    ( '82R5',      82.5 ),
    ( '84R5',      84.5 ),
    ( '86R6',      86.6 ),
    ( '88R7',      88.7 ),
    ( '90R9',      90.9 ),
    ( '91R0',      91.0 ),
    ( '93R1',      93.1 ),
    ( '95R3',      95.3 ),
    ( '97R0',      97.6 ),
    ( '1000',     100.0 ),
    ( '1020',     102.0 ),
    ( '1050',     105.0 ),
    ( '1070',     107.0 ),
    ( '1100',     110.0 ),
    ( '1130',     113.0 ),
    ( '1150',     115.0 ),
    ( '1180',     118.0 ),
    ( '1200',     120.0 ),
    ( '1210',     121.0 ),
    ( '1240',     124.0 ),
    ( '1270',     127.0 ),
    ( '1300',     130.0 ),
    ( '1330',     133.0 ),
    ( '1370',     137.0 ),
    ( '1400',     140.0 ),
    ( '1430',     143.0 ),
    ( '1470',     147.0 ),
    ( '1500',     150.0 ),
    ( '1540',     154.0 ),
    ( '1580',     158.0 ),
    ( '1600',     160.0 ),
    ( '1620',     162.0 ),
    ( '1650',     165.0 ),
    ( '1690',     169.0 ),
    ( '1740',     174.0 ),
    ( '1780',     178.0 ),
    ( '1800',     180.0 ),
    ( '1820',     182.0 ),
    ( '1870',     187.0 ),
    ( '1910',     191.0 ),
    ( '1960',     196.0 ),
    ( '2000',     200.0 ),
    ( '2050',     205.0 ),
    ( '2100',     210.0 ),
    ( '2150',     215.0 ),
    ( '2200',     220.0 ),
    ( '2210',     221.0 ),
    ( '2260',     226.0 ),
    ( '2320',     232.0 ),
    ( '2370',     237.0 ),
    ( '2400',     240.0 ),
    ( '2430',     243.0 ),
    ( '2490',     249.0 ),
    ( '2550',     255.0 ),
    ( '2610',     261.0 ),
    ( '2670',     267.0 ),
    ( '2700',     270.0 ),
    ( '2740',     274.0 ),
    ( '2800',     280.0 ),
    ( '2870',     287.0 ),
    ( '2940',     294.0 ),
    ( '3000',     300.0 ),
    ( '3010',     301.0 ),
    ( '3090',     309.0 ),
    ( '3160',     316.0 ),
    ( '3240',     324.0 ),
    ( '3300',     330.0 ),
    ( '3320',     332.0 ),
    ( '3400',     340.0 ),
    ( '3480',     348.0 ),
    ( '3570',     357.0 ),
    ( '3600',     360.0 ),
    ( '3650',     356.0 ),
    ( '3740',     374.0 ),
    ( '3830',     383.0 ),
    ( '3900',     390.0 ),
    ( '3920',     392.0 ),
    ( '4020',     402.0 ),
    ( '4120',     412.0 ),
    ( '4220',     422.0 ),
    ( '4300',     430.0 ),
    ( '4320',     432.0 ),
    ( '4420',     442.0 ),
    ( '4530',     453.0 ),
    ( '4640',     464.0 ),
    ( '4700',     470.0 ),
    ( '4750',     475.0 ),
    ( '4870',     487.0 ),
    ( '4990',     499.0 ),
    ( '5100',     510.0 ),
    ( '5110',     511.0 ),
    ( '5230',     523.0 ),
    ( '5360',     536.0 ),
    ( '5490',     549.0 ),
    ( '5600',     560.0 ),
    ( '5620',     562.0 ),
    ( '5760',     576.0 ),
    ( '5900',     590.0 ),
    ( '6040',     604.0 ),
    ( '6190',     619.0 ),
    ( '6200',     620.0 ),
    ( '6340',     634.0 ),
    ( '6490',     649.0 ),
    ( '6650',     665.0 ),
    ( '6800',     680.0 ),
    ( '6810',     681.0 ),
    ( '6980',     698.0 ),
    ( '7150',     715.0 ),
    ( '7320',     732.0 ),
    ( '7500',     750.0 ),
    ( '7680',     768.0 ),
    ( '7870',     787.0 ),
    ( '8060',     806.0 ),
    ( '8200',     820.0 ),
    ( '8250',     825.0 ),
    ( '8450',     845.0 ),
    ( '8660',     866.0 ),
    ( '8870',     887.0 ),
    ( '9090',     909.0 ),
    ( '9100',     910.0 ),
    ( '9310',     931.0 ),
    ( '9530',     953.0 ),
    ( '9760',     976.0 ),
    ( '1001',    1000.0 ),
    ( '1021',    1020.0 ),
    ( '1051',    1050.0 ),
    ( '1071',    1070.0 ),
    ( '1101',    1100.0 ),
    ( '1131',    1130.0 ),
    ( '1151',    1150.0 ),
    ( '1181',    1180.0 ),
    ( '1201',    1200.0 ),
    ( '1211',    1210.0 ),
    ( '1241',    1240.0 ),
    ( '1271',    1270.0 ),
    ( '1301',    1300.0 ),
    ( '1331',    1330.0 ),
    ( '1371',    1370.0 ),
    ( '1401',    1400.0 ),
    ( '1431',    1430.0 ),
    ( '1471',    1470.0 ),
    ( '1501',    1500.0 ),
    ( '1541',    1540.0 ),
    ( '1581',    1580.0 ),
    ( '1601',    1600.0 ),
    ( '1621',    1620.0 ),
    ( '1651',    1650.0 ),
    ( '1691',    1690.0 ),
    ( '1741',    1740.0 ),
    ( '1781',    1780.0 ),
    ( '1801',    1800.0 ),
    ( '1821',    1820.0 ),
    ( '1871',    1870.0 ),
    ( '1911',    1910.0 ),
    ( '1961',    1960.0 ),
    ( '2001',    2000.0 ),
    ( '2051',    2050.0 ),
    ( '2101',    2100.0 ),
    ( '2151',    2150.0 ),
    ( '2201',    2200.0 ),
    ( '2211',    2210.0 ),
    ( '2261',    2260.0 ),
    ( '2321',    2320.0 ),
    ( '2371',    2370.0 ),
    ( '2401',    2400.0 ),
    ( '2431',    2430.0 ),
    ( '2491',    2490.0 ),
    ( '2551',    2550.0 ),
    ( '2611',    2610.0 ),
    ( '2671',    2670.0 ),
    ( '2701',    2700.0 ),
    ( '2741',    2740.0 ),
    ( '2801',    2800.0 ),
    ( '2871',    2870.0 ),
    ( '2941',    2940.0 ),
    ( '3001',    3000.0 ),
    ( '3011',    3010.0 ),
    ( '3091',    3090.0 ),
    ( '3161',    3160.0 ),
    ( '3241',    3240.0 ),
    ( '3301',    3300.0 ),
    ( '3321',    3320.0 ),
    ( '3401',    3400.0 ),
    ( '3481',    3480.0 ),
    ( '3571',    3570.0 ),
    ( '3601',    3600.0 ),
    ( '3651',    3560.0 ),
    ( '3741',    3740.0 ),
    ( '3831',    3830.0 ),
    ( '3901',    3900.0 ),
    ( '3921',    3920.0 ),
    ( '4021',    4020.0 ),
    ( '4121',    4120.0 ),
    ( '4221',    4220.0 ),
    ( '4301',    4300.0 ),
    ( '4321',    4320.0 ),
    ( '4421',    4420.0 ),
    ( '4531',    4530.0 ),
    ( '4641',    4640.0 ),
    ( '4701',    4700.0 ),
    ( '4751',    4750.0 ),
    ( '4871',    4870.0 ),
    ( '4991',    4990.0 ),
    ( '5101',    5100.0 ),
    ( '5111',    5110.0 ),
    ( '5231',    5230.0 ),
    ( '5361',    5360.0 ),
    ( '5491',    5490.0 ),
    ( '5601',    5600.0 ),
    ( '5621',    5620.0 ),
    ( '5761',    5760.0 ),
    ( '5901',    5900.0 ),
    ( '6041',    6040.0 ),
    ( '6191',    6190.0 ),
    ( '6201',    6200.0 ),
    ( '6341',    6340.0 ),
    ( '6491',    6490.0 ),
    ( '6651',    6650.0 ),
    ( '6801',    6800.0 ),
    ( '6811',    6810.0 ),
    ( '6981',    6980.0 ),
    ( '7151',    7150.0 ),
    ( '7321',    7320.0 ),
    ( '7501',    7500.0 ),
    ( '7681',    7680.0 ),
    ( '7871',    7870.0 ),
    ( '8061',    8060.0 ),
    ( '8201',    8200.0 ),
    ( '8251',    8250.0 ),
    ( '8451',    8450.0 ),
    ( '8661',    8660.0 ),
    ( '8871',    8870.0 ),
    ( '9091',    9090.0 ),
    ( '9101',    9100.0 ),
    ( '9311',    9310.0 ),
    ( '9531',    9530.0 ),
    ( '9761',    9760.0 ),
    ( '1002',   10000.0 ),
    ( '1022',   10200.0 ),
    ( '1052',   10500.0 ),
    ( '1072',   10700.0 ),
    ( '1102',   11000.0 ),
    ( '1132',   11300.0 ),
    ( '1152',   11500.0 ),
    ( '1182',   11800.0 ),
    ( '1202',   12000.0 ),
    ( '1212',   12100.0 ),
    ( '1242',   12400.0 ),
    ( '1272',   12700.0 ),
    ( '1302',   13000.0 ),
    ( '1332',   13300.0 ),
    ( '1372',   13700.0 ),
    ( '1402',   14000.0 ),
    ( '1432',   14300.0 ),
    ( '1472',   14700.0 ),
    ( '1502',   15000.0 ),
    ( '1542',   15400.0 ),
    ( '1582',   15800.0 ),
    ( '1602',   16000.0 ),
    ( '1622',   16200.0 ),
    ( '1652',   16500.0 ),
    ( '1692',   16900.0 ),
    ( '1742',   17400.0 ),
    ( '1782',   17800.0 ),
    ( '1802',   18000.0 ),
    ( '1822',   18200.0 ),
    ( '1872',   18700.0 ),
    ( '1912',   19100.0 ),
    ( '1962',   19600.0 ),
    ( '2002',   20000.0 ),
    ( '2052',   20500.0 ),
    ( '2102',   21000.0 ),
    ( '2152',   21500.0 ),
    ( '2202',   22000.0 ),
    ( '2212',   22100.0 ),
    ( '2262',   22600.0 ),
    ( '2322',   23200.0 ),
    ( '2372',   23700.0 ),
    ( '2402',   24000.0 ),
    ( '2432',   24300.0 ),
    ( '2492',   24900.0 ),
    ( '2552',   25500.0 ),
    ( '2612',   26100.0 ),
    ( '2672',   26700.0 ),
    ( '2702',   27000.0 ),
    ( '2742',   27400.0 ),
    ( '2802',   28000.0 ),
    ( '2872',   28700.0 ),
    ( '2942',   29400.0 ),
    ( '3002',   30000.0 ),
    ( '3012',   30100.0 ),
    ( '3092',   30900.0 ),
    ( '3162',   31600.0 ),
    ( '3242',   32400.0 ),
    ( '3302',   33000.0 ),
    ( '3322',   33200.0 ),
    ( '3402',   34000.0 ),
    ( '3482',   34800.0 ),
    ( '3572',   35700.0 ),
    ( '3602',   36000.0 ),
    ( '3652',   35600.0 ),
    ( '3742',   37400.0 ),
    ( '3832',   38300.0 ),
    ( '3902',   39000.0 ),
    ( '3922',   39200.0 ),
    ( '4022',   40200.0 ),
    ( '4122',   41200.0 ),
    ( '4222',   42200.0 ),
    ( '4302',   43000.0 ),
    ( '4322',   43200.0 ),
    ( '4422',   44200.0 ),
    ( '4532',   45300.0 ),
    ( '4642',   46400.0 ),
    ( '4702',   47000.0 ),
    ( '4752',   47500.0 ),
    ( '4872',   48700.0 ),
    ( '4992',   49900.0 ),
    ( '5102',   51000.0 ),
    ( '5112',   51100.0 ),
    ( '5232',   52300.0 ),
    ( '5362',   53600.0 ),
    ( '5492',   54900.0 ),
    ( '5602',   56000.0 ),
    ( '5622',   56200.0 ),
    ( '5762',   57600.0 ),
    ( '5902',   59000.0 ),
    ( '6042',   60400.0 ),
    ( '6192',   61900.0 ),
    ( '6202',   62000.0 ),
    ( '6342',   63400.0 ),
    ( '6492',   64900.0 ),
    ( '6652',   66500.0 ),
    ( '6802',   68000.0 ),
    ( '6812',   68100.0 ),
    ( '6982',   69800.0 ),
    ( '7152',   71500.0 ),
    ( '7322',   73200.0 ),
    ( '7502',   75000.0 ),
    ( '7682',   76800.0 ),
    ( '7872',   78700.0 ),
    ( '8062',   80600.0 ),
    ( '8202',   82000.0 ),
    ( '8252',   82500.0 ),
    ( '8452',   84500.0 ),
    ( '8662',   86600.0 ),
    ( '8872',   88700.0 ),
    ( '9092',   90900.0 ),
    ( '9102',   91000.0 ),
    ( '9312',   93100.0 ),
    ( '9532',   95300.0 ),
    ( '9762',   97600.0 ),
    ( '1003',  100000.0 ),
    ( '1023',  102000.0 ),
    ( '1053',  105000.0 ),
    ( '1073',  107000.0 ),
    ( '1103',  110000.0 ),
    ( '1133',  113000.0 ),
    ( '1153',  115000.0 ),
    ( '1183',  118000.0 ),
    ( '1203',  120000.0 ),
    ( '1213',  121000.0 ),
    ( '1243',  124000.0 ),
    ( '1273',  127000.0 ),
    ( '1303',  130000.0 ),
    ( '1333',  133000.0 ),
    ( '1373',  137000.0 ),
    ( '1403',  140000.0 ),
    ( '1433',  143000.0 ),
    ( '1473',  147000.0 ),
    ( '1503',  150000.0 ),
    ( '1543',  154000.0 ),
    ( '1583',  158000.0 ),
    ( '1603',  160000.0 ),
    ( '1623',  162000.0 ),
    ( '1653',  165000.0 ),
    ( '1693',  169000.0 ),
    ( '1743',  174000.0 ),
    ( '1783',  178000.0 ),
    ( '1803',  180000.0 ),
    ( '1823',  182000.0 ),
    ( '1873',  187000.0 ),
    ( '1913',  191000.0 ),
    ( '1963',  196000.0 ),
    ( '2003',  200000.0 ),
    ( '2053',  205000.0 ),
    ( '2103',  210000.0 ),
    ( '2153',  215000.0 ),
    ( '2203',  220000.0 ),
    ( '2213',  221000.0 ),
    ( '2263',  226000.0 ),
    ( '2323',  232000.0 ),
    ( '2373',  237000.0 ),
    ( '2403',  240000.0 ),
    ( '2433',  243000.0 ),
    ( '2493',  249000.0 ),
    ( '2553',  255000.0 ),
    ( '2613',  261000.0 ),
    ( '2673',  267000.0 ),
    ( '2703',  270000.0 ),
    ( '2743',  274000.0 ),
    ( '2803',  280000.0 ),
    ( '2873',  287000.0 ),
    ( '2943',  294000.0 ),
    ( '3003',  300000.0 ),
    ( '3013',  301000.0 ),
    ( '3093',  309000.0 ),
    ( '3163',  316000.0 ),
    ( '3243',  324000.0 ),
    ( '3303',  330000.0 ),
    ( '3323',  332000.0 ),
    ( '3403',  340000.0 ),
    ( '3483',  348000.0 ),
    ( '3573',  357000.0 ),
    ( '3603',  360000.0 ),
    ( '3653',  356000.0 ),
    ( '3743',  374000.0 ),
    ( '3833',  383000.0 ),
    ( '3903',  390000.0 ),
    ( '3923',  392000.0 ),
    ( '4023',  402000.0 ),
    ( '4123',  412000.0 ),
    ( '4223',  422000.0 ),
    ( '4303',  430000.0 ),
    ( '4323',  432000.0 ),
    ( '4423',  442000.0 ),
    ( '4533',  453000.0 ),
    ( '4643',  464000.0 ),
    ( '4703',  470000.0 ),
    ( '4753',  475000.0 ),
    ( '4873',  487000.0 ),
    ( '4993',  499000.0 ),
    ( '5103',  510000.0 ),
    ( '5113',  511000.0 ),
    ( '5233',  523000.0 ),
    ( '5363',  536000.0 ),
    ( '5493',  549000.0 ),
    ( '5603',  560000.0 ),
    ( '5623',  562000.0 ),
    ( '5763',  576000.0 ),
    ( '5903',  590000.0 ),
    ( '6043',  604000.0 ),
    ( '6193',  619000.0 ),
    ( '6203',  620000.0 ),
    ( '6343',  634000.0 ),
    ( '6493',  649000.0 ),
    ( '6653',  665000.0 ),
    ( '6803',  680000.0 ),
    ( '6813',  681000.0 ),
    ( '6983',  698000.0 ),
    ( '7153',  715000.0 ),
    ( '7323',  732000.0 ),
    ( '7503',  750000.0 ),
    ( '7683',  768000.0 ),
    ( '7873',  787000.0 ),
    ( '8063',  806000.0 ),
    ( '8203',  820000.0 ),
    ( '8253',  825000.0 ),
    ( '8453',  845000.0 ),
    ( '8663',  866000.0 ),
    ( '8873',  887000.0 ),
    ( '9093',  909000.0 ),
    ( '9103',  910000.0 ),
    ( '9313',  931000.0 ),
    ( '9533',  953000.0 ),
    ( '9763',  976000.0 ),
    ( '1004', 1000000.0 )
    ]

for package in packages:
    for resistance in resistances:
        print "CALL AddResistor("
        print "    'Bourns',"
        print "    'CR%s-FX-%sELF'," % ( package[0], resistance[0] )
        print "    '%s'," % package[1]
        print "    'RESISTOR',"
        print "    %f," % resistance[1]
        print "    %f" % 0.01
        print "    );"
        print

##############################################################################
##
##  CD0603/1005-Z Surface Mount Zener Diode Series
##
##############################################################################

packages = [
    ( '0603', '0603(1608)' ),
    ( '1005', '1005(2512)' )
    ]

voltages = [
    ( '2',    2.0 ),
    ( '2V2',  2.2 ),
    ( '2V4',  2.4 ),
    ( '2V7',  2.7 ),
    ( '3',    3.0 ),
    ( '3V3',  3.3 ),
    ( '3V6',  3.6 ),
    ( '3V9',  3.9 ),
    ( '4V3',  4.3 ),
    ( '4V7',  4.7 ),
    ( '5V1',  5.1 ),
    ( '5V6',  5.6 ),
    ( '6V2',  6.2 ),
    ( '6V8',  6.8 ),
    ( '7V5',  7.5 ),
    ( '8V2',  8.2 ),
    ( '9V1',  9.1 ),
    ( '10',  10.0 ),
    ( '11',  11.0 ),
    ( '12',  12.0 ),
    ( '13',  13.0 ),
    ( '15',  15.0 ),
    ( '16',  16.0 ),
    ( '18',  18.0 ),
    ( '20',  20.0 ),
    ( '22',  22.0 ),
    ( '24',  24.0 ),
    ( '27',  27.0 ),
    ( '30',  30.0 ),
    ( '33',  33.0 ),
    ( '36',  36.0 ),
    ( '39',  39.0 )
    ]

for package in packages:
    for voltage in voltages:
        print "CALL AddZenerDiode("
        print "    'Bourns',"
        print "    'CD%s-Z%s'," % ( package[0], voltage[0] )
        print "    '%s'," % package[1]
        print "    'ZENER_DIODE',"
        print "    %f" % voltage[1]
        print "    );"
        print

