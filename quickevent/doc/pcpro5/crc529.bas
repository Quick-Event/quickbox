Public Function crc(ByVal s As String) As Long

Const poly = &H8005&
Const BITF = &H8000&

Dim i As Long, j As Long, tmp As Long, val As Long

If s = "" Or Len(s) < 2 Then
    crc = 0
Else

i = 1
tmp = 0
tmp = Asc(Mid(s, i, 1))
tmp = tmp * 256
i = i + 1
tmp = tmp + Asc(Mid(s, i, 1))

If Len(s) = 2 Then
    crc = tmp
Else
    For i = 3 To Len(s) + 2  'Alle Byte+ 2Byte 0 am Ende
        If i < Len(s) Then
            val = Asc(Mid(s, i, 1))
            val = val * 256
            i = i + 1
        val = val + Asc(Mid(s, i, 1))
        Else
          If i = Len(s) Then
            val = Asc(Mid(s, i, 1))
            val = val * 256 'Es war ungerade Anzahl von Bytes
          Else
            val = 0 'letzte Werte mit XOR mit 0
          End If
          i = i + 2 'Abbruch für Schleife
        End If
    'Berechnen des CRC
      For j = 0 To 15
         If tmp And BITF Then
            tmp = (tmp + tmp) And &HFFFF&
            If val And BITF Then
              tmp = (tmp + 1) And &HFFFF&
            End If
            tmp = (tmp Xor poly) And &HFFFF&
         Else
            tmp = (tmp + tmp) And &HFFFF&
            If val And BITF Then
              tmp = (tmp + 1) And &HFFFF&
            End If
         End If
         val = (val + val) And &HFFFF&
       Next
    Next
    crc = tmp
    End If
End If
End Function