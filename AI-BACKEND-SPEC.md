# AI backend contract (future implementation)

## Request
`POST /v1/decoration-plan`
`Content-Type: application/json`

Required fields:
- prompt: natural-language request
- style: Minimal, Cartoon, Realistic, or Pixel art
- detail: integer 0..100
- maxObjects: integer 1..750
- selectionBounds: x, y, width, height

## Response
Return JSON only:
```json
{
  "objects": [
    {"objectId": 1, "x": 10, "y": 20, "scale": 1.0, "rotation": 0}
  ],
  "summary": "Mossy stone wall with a broken central window"
}
```

Server and client must validate object count, numeric ranges, finite coordinates,
allowed object IDs, selection bounds, and response size. The server should never
return code for execution. The mod must display a preview and require confirmation.
